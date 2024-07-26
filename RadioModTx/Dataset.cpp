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
Dataset.cpp

This file contains the sources for dataset.
*/

#include "Dataset.h"


Dataset* Dataset::sInstance = nullptr;

const std::map<Dataset::DatasetSource, uint16_t> Dataset::FRAME_LENGTH =
{
    { Dataset::DATASET_SOURCE_RADIOML_2016_10A,   128 },
    { Dataset::DATASET_SOURCE_RADIOML_2018_01,   1024 },
    { Dataset::DATASET_SOURCE_HISARMOD_2019_1,   1024 }
};

const std::map<Dataset::DatasetSource, uint16_t> Dataset::FRAMES_PER_MOD_SNR_NR =
{
    { Dataset::DATASET_SOURCE_RADIOML_2016_10A,  1000 },
    { Dataset::DATASET_SOURCE_RADIOML_2018_01,   4096 },
    { Dataset::DATASET_SOURCE_HISARMOD_2019_1,    500 }
};

const std::map<Dataset::DatasetSource, uint8_t> Dataset::MODULATIONS_NR =
{
    { Dataset::DATASET_SOURCE_RADIOML_2016_10A,  11 },
    { Dataset::DATASET_SOURCE_RADIOML_2018_01,   24 },
    { Dataset::DATASET_SOURCE_HISARMOD_2019_1,   26 }
};

const std::map<Dataset::DatasetSource, uint8_t> Dataset::SNRS_NR =
{
    { Dataset::DATASET_SOURCE_RADIOML_2016_10A,  20 },
    { Dataset::DATASET_SOURCE_RADIOML_2018_01,   26 },
    { Dataset::DATASET_SOURCE_HISARMOD_2019_1,   20 }
};

//!************************************************************************
//! Constructor
//!************************************************************************
Dataset::Dataset()
    : mSource( DATASET_SOURCE_RADIOML_2016_10A )
{
}


//!************************************************************************
//! Singleton
//!
//! @returns the instance of the object
//!************************************************************************
Dataset* Dataset::getInstance()
{
    if( !sInstance )
    {
        sInstance = new Dataset;
    }

    return sInstance;
}


//!************************************************************************
//! Get the address of the dataset source
//!
//! @returns The dataset source address
//!************************************************************************
Dataset::DatasetSource& Dataset::getSource()
{
    return mSource;
}
