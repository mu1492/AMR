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
Dataset.h

This file contains the definitions for dataset.
*/

#ifndef Dataset_h
#define Dataset_h

#include "Modulation.h"

#include <cstdint>
#include <list>
#include <map>
#include <vector>


//************************************************************************
// Class for handling the dataset
//************************************************************************
class Dataset
{
    //************************************************************************
    // constants and types
    //************************************************************************
    public:
        typedef enum : uint8_t
        {
            DATASET_SOURCE_RADIOML_2016_10A,
            DATASET_SOURCE_RADIOML_2018_01,
            DATASET_SOURCE_HISARMOD_2019_1
        }DatasetSource;

        // number of (I,Q) pairs per frame
        static const std::map<DatasetSource, uint16_t> FRAME_LENGTH;

        // number of frames per modulation-SNR combination
        static const std::map<DatasetSource, uint16_t> FRAMES_PER_MOD_SNR_NR;

        // total number of modulations (analog + digital)
        static const std::map<DatasetSource, uint8_t> MODULATIONS_NR;

        // number of different SNRs
        static const std::map<DatasetSource, uint8_t> SNRS_NR;

        typedef struct
        {
            float i;
            float q;
        }IQPoint;

        typedef std::vector<IQPoint>                            FrameData;

        typedef struct
        {
            std::vector<FrameData>  frameDataVec;
            float                   maxVal;
        }SignalData;

        typedef std::pair<Modulation::ModulationName, int>      ModulationSnrPair;
        typedef std::map<ModulationSnrPair, SignalData>         ModulationSnrSignalDataMap;


    //************************************************************************
    // functions
    //************************************************************************
    public:
        Dataset();

        static Dataset* getInstance();

        DatasetSource& getSource();


    //************************************************************************
    // variables
    //************************************************************************
    private:
        static Dataset*     sInstance;                  //!< singleton

        DatasetSource       mSource;                    //!< dataset source

        uint8_t             mNrOfModulations;           //!< total number of modulations
        uint8_t             mNrOfAnalogModulations;     //!< number of analog modulations
        uint8_t             mNrOfDigitalModulations;    //!< number of digital modulations

        uint8_t             mNrOfDifferentSnrs;         //!< total number of different SNRs

        uint32_t            mNrOfFrames;                //!< total number of frames
        uint16_t            mFrameLength;               //!< frame length

        ModulationSnrSignalDataMap  mModulationSnrSignalDataMap;  //!< map with data signals for modulation-SNR combinations
};

#endif // Dataset_h
