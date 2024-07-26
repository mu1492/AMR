///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2024 Mihai Ursu                                                 //
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
// (at your option) any later version.                                           //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

/*
CsvParser.cpp

This file contains the sources for CSV parser.
*/

#include "CsvParser.h"

#include <cmath>
#include <fstream>
#include <iostream>


const std::map<int, Modulation::ModulationName> CsvParser::MODULATION_MAPPING =
{
    //*********
    // digital
    //*********
    // PSK
    {  0, Modulation::NAME_BPSK },
    { 10, Modulation::NAME_QPSK },
    { 20, Modulation::NAME_8PSK },
    { 30, Modulation::NAME_16PSK },
    { 40, Modulation::NAME_32PSK },
    { 50, Modulation::NAME_64PSK },
    // QAM
    {  1, Modulation::NAME_4QAM },
    { 11, Modulation::NAME_8QAM },
    { 21, Modulation::NAME_16QAM },
    { 31, Modulation::NAME_32QAM },
    { 41, Modulation::NAME_64QAM },
    { 51, Modulation::NAME_128QAM },
    { 61, Modulation::NAME_256QAM },
    // FSK
    {  2, Modulation::NAME_2FSK },
    { 12, Modulation::NAME_4FSK },
    { 22, Modulation::NAME_8FSK },
    { 32, Modulation::NAME_16FSK },
    // PAM
    {  3, Modulation::NAME_4PAM },
    { 13, Modulation::NAME_8PAM },
    { 23, Modulation::NAME_16PAM },
    //*********
    // analog
    //*********
    {  4, Modulation::NAME_AM_DSB },
    { 14, Modulation::NAME_AM_DSB_SC },
    { 24, Modulation::NAME_AM_USB },
    { 34, Modulation::NAME_AM_LSB },
    { 44, Modulation::NAME_FM },
    { 54, Modulation::NAME_PM }
};

const std::vector<int> CsvParser::MODULATION_SERIES =
{
     4, 14, 44, 32,  2, 12, 22, 34, 23,  3,
    13, 54, 30,  0, 40, 10, 50, 20, 51, 21,
    61, 31,  1, 41, 11, 24
};

//!************************************************************************
//! Constructor
//!************************************************************************
CsvParser::CsvParser()
{
}


//!************************************************************************
//! Get a complex number (I,Q) from a string like "I+Qi"
//!
//! @returns The point represented by its I and Q parts
//!************************************************************************
Dataset::IQPoint CsvParser::getPoint
    (
    std::string aString     //!< string with complex number
    ) const
{
    Dataset::IQPoint iqPoint = { 0, 0 };   
    size_t sepSignIndex = aString.find( "+", 1 );

    if( std::string::npos == sepSignIndex )
    {
        sepSignIndex = aString.find( "-", 1 );
    }

    if( std::string::npos != sepSignIndex )
    {
        std::string dblStr = aString.substr( 0, sepSignIndex );
        iqPoint.i = ::atof( dblStr.c_str() );

        dblStr = aString.substr( sepSignIndex, aString.size() - sepSignIndex - 1 );
        iqPoint.q = ::atof( dblStr.c_str() );
    }

    return iqPoint;
}


//!************************************************************************
//! Parse a CSV file using the HisarMod 2019.1 dataset syntax
//!
//! @returns nothing
//!************************************************************************
/* slot */ void CsvParser::parseDataset()
{
    mUniqueModVec.clear();
    mUniqueSnrVec.clear();
    mMap.clear();

    std::ifstream inputFile( mFileName );
    bool parseFailed = false;

    if( inputFile.is_open() )
    {
        size_t crtLineNr = 0;
        std::string currentLine;

        int crtSnrDb = -100;
        int oldSnrDb = crtSnrDb;

        size_t crtModInt = SIZE_MAX;
        size_t oldModInt = crtModInt;        
        Modulation::ModulationName modName = Modulation::NAME_UNKNOWN;

        bool newCombination = false;
        Dataset::ModulationSnrPair modSnrPair;

        Dataset::SignalData signalData;
        Dataset::FrameData frameData;
        Dataset::IQPoint iqPoint = { 0, 0 };

        const size_t NR_LINES_PER_SNR = Dataset::FRAMES_PER_MOD_SNR_NR.at( Dataset::DATASET_SOURCE_HISARMOD_2019_1 )
                                      * Dataset::MODULATIONS_NR.at( Dataset::DATASET_SOURCE_HISARMOD_2019_1 );        

        while( std::getline( inputFile, currentLine ) )
        {
            crtSnrDb = -20 + 2 * ( crtLineNr / NR_LINES_PER_SNR );

            crtModInt = ( crtLineNr % NR_LINES_PER_SNR ) / Dataset::FRAMES_PER_MOD_SNR_NR.at( Dataset::DATASET_SOURCE_HISARMOD_2019_1 );
            crtModInt = MODULATION_SERIES.at( crtModInt );

            modName = MODULATION_MAPPING.at( crtModInt );

            newCombination = ( crtSnrDb != oldSnrDb || crtModInt != oldModInt );

            if( newCombination )
            {
                mUniqueModVec.push_back( modName );
                mUniqueSnrVec.push_back( crtSnrDb );

                modSnrPair = std::make_pair( modName, crtSnrDb );

                signalData.frameDataVec.clear();
                signalData.maxVal = 0;
            }

            const size_t DATA_STR_LEN = currentLine.size();
            frameData.clear();

            for( size_t j = 0; j < DATA_STR_LEN; j++ )
            {
                size_t commaIndex = currentLine.find( ",", j + 1 );
                std::string cxStr;

                if( std::string::npos != commaIndex )
                {
                    cxStr = currentLine.substr( j, commaIndex - j );
                    iqPoint = getPoint( cxStr );
                    frameData.push_back( iqPoint );

                    if( fabs( iqPoint.i ) > signalData.maxVal )
                    {
                        signalData.maxVal = fabs( iqPoint.i );
                    }

                    if( fabs( iqPoint.q ) > signalData.maxVal )
                    {
                        signalData.maxVal = fabs( iqPoint.q );
                    }

                    j = commaIndex;
                }
                else // last value
                {
                    cxStr = currentLine.substr( j, DATA_STR_LEN - j );
                    iqPoint = getPoint( cxStr );
                    frameData.push_back( iqPoint );

                    if( fabs( iqPoint.i ) > signalData.maxVal )
                    {
                        signalData.maxVal = fabs( iqPoint.i );
                    }

                    if( fabs( iqPoint.q ) > signalData.maxVal )
                    {
                        signalData.maxVal = fabs( iqPoint.q );
                    }

                    j = DATA_STR_LEN;
                }
            }

            if( Dataset::FRAME_LENGTH.at( Dataset::DATASET_SOURCE_HISARMOD_2019_1 ) != frameData.size() )
            {
                parseFailed = true;
                break;
            }

            signalData.frameDataVec.push_back( frameData );

            if( crtLineNr
           && ( 0 == ( crtLineNr + 1 ) % Dataset::FRAMES_PER_MOD_SNR_NR.at( Dataset::DATASET_SOURCE_HISARMOD_2019_1 ) )
              )
            {
                if( Dataset::FRAMES_PER_MOD_SNR_NR.at( Dataset::DATASET_SOURCE_HISARMOD_2019_1 ) != signalData.frameDataVec.size() )
                {
                    parseFailed = true;
                    break;
                }

                mMap.insert( std::pair<Dataset::ModulationSnrPair, Dataset::SignalData>( modSnrPair, signalData ) );
            }

            oldSnrDb = crtSnrDb;
            oldModInt = crtModInt;

            crtLineNr++;
        }

        inputFile.close();
    }

    removeDuplicates( mUniqueModVec );
    removeDuplicates( mUniqueSnrVec );

    if( Dataset::MODULATIONS_NR.at( Dataset::DATASET_SOURCE_HISARMOD_2019_1 ) != mUniqueModVec.size()
     || Dataset::SNRS_NR.at( Dataset::DATASET_SOURCE_HISARMOD_2019_1 ) != mUniqueSnrVec.size()
      )
    {
        parseFailed = true;
    }

    mStatus = !parseFailed;
    emit parseFinished();
}


//!************************************************************************
//! Parse a dataset looking for a single modulation
//!
//! @returns nothing
//!************************************************************************
/* slot */ void CsvParser::parseDatasetSingleModulation()
{
}
