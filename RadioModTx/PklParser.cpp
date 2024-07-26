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
PklParser.cpp

This file contains the sources for PKL parser.
*/

#include "PklParser.h"

#include "chooseser.h"

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <utility>


//!************************************************************************
//! Constructor
//!************************************************************************
PklParser::PklParser()
{
}


//!************************************************************************
//! Parse a pickle file using the RadioML 2016.10A dataset syntax
//!
//! @returns nothing
//!************************************************************************
/* slot */ void PklParser::parseDataset()
{
    mUniqueModVec.clear();
    mUniqueSnrVec.clear();
    mMap.clear();

    Val pklResult;

    try
    {
        LoadValFromFile( mFileName, pklResult );
    }
    catch(...)
    {
        pklResult = "";
    }

    const std::string PKL_STR = static_cast<std::string>( pklResult );
    const size_t PKL_STR_LEN = PKL_STR.size();
    bool parseFailed = ( 0 == PKL_STR_LEN );
    bool inModSnr = false;
    bool inArray = false;
    size_t closingModSnrIndex = 0;
    size_t closingArrayIndex = 0;

    const size_t EXPECTED_DBL_VALUES = Dataset::FRAME_LENGTH.at( Dataset::DATASET_SOURCE_RADIOML_2016_10A )
                                     * Dataset::FRAMES_PER_MOD_SNR_NR.at( Dataset::DATASET_SOURCE_RADIOML_2016_10A )
                                     * 2;

    Modulation* modInstance = Modulation::getInstance();
    Dataset::ModulationSnrPair modSnrPair;

    for( size_t i = 0; i < PKL_STR_LEN; i++ )
    {
        if( '(' == PKL_STR.at( i ) && !inModSnr && !inArray )
        {
            inModSnr = true;

            closingModSnrIndex = PKL_STR.find( ")", i + 1 );
            std::string modSnrStr = PKL_STR.substr( i + 1, closingModSnrIndex - i - 1 );
            size_t startModIndex = modSnrStr.find( "'" );
            Modulation::ModulationName modName = Modulation::NAME_UNKNOWN;
            int snrDb = -100;

            if( std::string::npos != startModIndex )
            {
                size_t closingModIndex = modSnrStr.find( "'", startModIndex + 1 );

                if( std::string::npos != closingModIndex )
                {
                    std::string modStr = modSnrStr.substr( startModIndex + 1, closingModIndex - startModIndex - 1 );
                    modName = modInstance->getModulationName( modStr );

                    const std::string SEPARATOR_STR = ", ";
                    size_t separatorIndex = modSnrStr.find( SEPARATOR_STR );

                    if( std::string::npos != separatorIndex )
                    {
                        const size_t START_INDEX = separatorIndex + SEPARATOR_STR.size();
                        std::string snrStr = modSnrStr.substr( START_INDEX, modSnrStr.size() - START_INDEX );
                        snrDb = atoi( snrStr.c_str() );
                    }
                    else
                    {
                        parseFailed = true;
                        break;
                    }
                }
                else
                {
                    parseFailed = true;
                    break;
                }
            }
            else
            {
                parseFailed = true;
                break;
            }

            mUniqueModVec.push_back( modName );
            mUniqueSnrVec.push_back( snrDb );

            modSnrPair = std::make_pair( modName, snrDb );
            i = closingModSnrIndex;
        }
        else if( '(' == PKL_STR.at( i ) && inModSnr && !inArray )
        {
            inArray = true;

            closingArrayIndex = PKL_STR.find( ")", i + 1 );
            std::string arrayStr = PKL_STR.substr( i + 1, closingArrayIndex - i - 1 );

            size_t startArrayIndex = arrayStr.find( "[" );

            if( std::string::npos != startArrayIndex )
            {
                size_t closingArrayIndex = arrayStr.find( "]", startArrayIndex + 1 );

                if( std::string::npos != closingArrayIndex )
                {
                    std::vector<float> fltVec;
                    const std::string DATA_STR = arrayStr.substr( startArrayIndex + 1, closingArrayIndex - startArrayIndex - 1 );
                    const size_t DATA_STR_LEN = DATA_STR.size();

                    for( size_t j = 0; j < DATA_STR_LEN; j++ )
                    {
                        size_t commaIndex = DATA_STR.find( ",", j + 1 );
                        std::string fltStr;
                        float fltValue = 0;

                        if( std::string::npos != commaIndex )
                        {
                            fltStr = DATA_STR.substr( j, commaIndex - j );
                            fltValue = ::atof( fltStr.c_str() );
                            fltVec.push_back( fltValue );

                            j = commaIndex;
                        }
                        else // last value
                        {
                            fltStr = DATA_STR.substr( j, DATA_STR_LEN - j );
                            fltValue = ::atof( fltStr.c_str() );
                            fltVec.push_back( fltValue );

                            j = DATA_STR_LEN;
                        }
                    }

                    if( EXPECTED_DBL_VALUES != fltVec.size() )
                    {
                        parseFailed = true;
                        break;
                    }
                    else
                    {
                        Dataset::SignalData signalData;
                        signalData.maxVal = 0;

                        for( size_t crtFrame = 0;
                             crtFrame < Dataset::FRAMES_PER_MOD_SNR_NR.at( Dataset::DATASET_SOURCE_RADIOML_2016_10A );
                             crtFrame++ )
                        {
                            Dataset::FrameData frameData;

                            for( size_t crtPoint = 0;
                                 crtPoint < Dataset::FRAME_LENGTH.at( Dataset::DATASET_SOURCE_RADIOML_2016_10A );
                                 crtPoint++ )
                            {
                                size_t pointIndex = crtPoint + crtFrame * 2 * Dataset::FRAME_LENGTH.at( Dataset::DATASET_SOURCE_RADIOML_2016_10A );
                                float iPart = fltVec.at( pointIndex );
                                float qPart = fltVec.at( pointIndex + Dataset::FRAME_LENGTH.at( Dataset::DATASET_SOURCE_RADIOML_2016_10A ) );
                                Dataset::IQPoint iqPoint = { iPart, qPart };
                                frameData.push_back( iqPoint );

                                if( fabs( iPart ) > signalData.maxVal )
                                {
                                    signalData.maxVal = fabs( iPart );
                                }

                                if( fabs( qPart ) > signalData.maxVal )
                                {
                                    signalData.maxVal = fabs( qPart );
                                }
                            }

                            if( Dataset::FRAME_LENGTH.at( Dataset::DATASET_SOURCE_RADIOML_2016_10A ) != frameData.size() )
                            {
                                parseFailed = true;
                                break;
                            }

                            signalData.frameDataVec.push_back( frameData );
                        }

                        if( parseFailed )
                        {
                            break;
                        }

                        if( Dataset::FRAMES_PER_MOD_SNR_NR.at( Dataset::DATASET_SOURCE_RADIOML_2016_10A ) != signalData.frameDataVec.size() )
                        {
                            parseFailed = true;
                            break;
                        }

                        mMap.insert( std::pair<Dataset::ModulationSnrPair, Dataset::SignalData>( modSnrPair, signalData ) );
                    }
                }
                else
                {
                    parseFailed = true;
                    break;
                }
            }
            else
            {
                parseFailed = true;
                break;
            }

            i = closingArrayIndex;

            inModSnr = false;
            inArray = false;
        }
    }

    removeDuplicates( mUniqueModVec );
    removeDuplicates( mUniqueSnrVec );

    if( Dataset::MODULATIONS_NR.at( Dataset::DATASET_SOURCE_RADIOML_2016_10A ) != mUniqueModVec.size()
     || Dataset::SNRS_NR.at( Dataset::DATASET_SOURCE_RADIOML_2016_10A ) != mUniqueSnrVec.size()
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
/* slot */ void PklParser::parseDatasetSingleModulation()
{
}
