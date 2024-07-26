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
RadioModTx.h

This file contains the definitions for radio modulation transmitter.
*/

#ifndef RadioModTx_h
#define RadioModTx_h

#include "Dataset.h"
#include "CsvParser.h"
#include "Hdf5Parser.h"
#include "Modulation.h"
#include "PklParser.h"
#include "TxHal.h"

#include <QMainWindow>
#include <QThread>

QT_BEGIN_NAMESPACE
    namespace Ui
    {
        class RadioModTx;
    }
QT_END_NAMESPACE


//************************************************************************
// Class for handling the radio modulation transmitter
//************************************************************************
class RadioModTx : public QMainWindow
{
    Q_OBJECT

    //************************************************************************
    // constants and types
    //************************************************************************


    //************************************************************************
    // functions
    //************************************************************************
    public:
        RadioModTx
            (
            QWidget* aParent = nullptr //!< parent widget
            );

        ~RadioModTx();

    private:
        std::string makeDumpFilename() const;

        void updateModulationControls();

        void updateSnrControls();

        void updateTxControls();

        void updateTxList();

    private slots:
        void handleChangedTxFlo
            (
            double aFrequencyMhz    //!< LO frequency [MHz]
            );

        void handleChangedTxNcoGain
            (
            double aGainScale       //!< gain scale [0..1]
            );

        void handleDatasetParseFinished();

        void handleModulationNameChanged
            (
            int aIndex  //!< index
            );

        void handleModulationSnrChanged
            (
            int aIndex  //!< index
            );

        void handleStartTxStreaming();

        void handleStopTxStreaming();

        void handleTxChanged
            (
            int aIndex  //!< index
            );

        void openDatasetSrc();

        void updateControlsParseFinished();

        void updateControlsParseStarted();

        void updateDatasetSrc
            (
            int aIndex  //!< index
            );


    //************************************************************************
    // variables
    //************************************************************************
    private:
        Ui::RadioModTx*                         mMainUi;                //!< main UI

        Modulation*                             mModulationInstance;    //!< modulation instance

        Dataset*                                mDatasetInstance;       //!< dataset instance
        Dataset::DatasetSource                  mDatasetType;           //!< dataset type

        PklParser*                              mPklParser;             //!< PKL parser
        Hdf5Parser*                             mHdf5Parser;            //!< HDF5 parser
        CsvParser*                              mCsvParser;             //!< CSV parser

        QThread*                                mPklParserThread;       //!< PKL parser thread
        QThread*                                mHdf5ParserThread;      //!< HDF5 parser thread
        QThread*                                mCsvParserThread;       //!< CSV parser thread

        bool                                    mParserStatus;          //!< true if parser was successful

        TxHal*                                  mTxHalInstance;         //!< Tx HAL
        int                                     mTxIioScanIndex;        //!< IIO scan context for Tx

        std::vector<Modulation::ModulationName> mUniqueModVec;          //!< vector with unique modulations
        std::vector<int>                        mUniqueSnrVec;          //!< vector with unique SNRs
        Dataset::ModulationSnrSignalDataMap     mMap;                   //!< map with data signals for modulation-SNR combinations

        Modulation::ModulationName              mCrtModulation;         //!< selected modulation
        int                                     mCrtSnrDb;              //!< selected SNR [dB]
};
#endif // RadioModTx_h
