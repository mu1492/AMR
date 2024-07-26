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
RadioModTx.cpp

This file contains the sources for radio modulation transmitter.
*/

#include "RadioModTx.h"
#include "./ui_RadioModTx.h"

#include <fstream>
#include <iostream>
#include <string>

#include <QButtonGroup>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>


//!************************************************************************
//! Constructor
//!************************************************************************
RadioModTx::RadioModTx
    (
    QWidget*    aParent //!< parent widget
    )
    : QMainWindow( aParent )
    , mMainUi( new Ui::RadioModTx )
    , mDatasetType( Dataset::DATASET_SOURCE_RADIOML_2016_10A )
    , mPklParser( new PklParser() )
    , mHdf5Parser( new Hdf5Parser() )
    , mCsvParser( new CsvParser() )
    , mPklParserThread( new QThread() )
    , mHdf5ParserThread( new QThread() )
    , mCsvParserThread( new QThread() )
    , mParserStatus( false )
    , mTxIioScanIndex( -1 )
{
    mMainUi->setupUi( this );

    //*************************
    // dataset
    //*************************
    mDatasetInstance = Dataset::getInstance();

    QButtonGroup* datasetBtnGroup = new QButtonGroup( this );
    datasetBtnGroup->addButton( mMainUi->DatasetRadioML2016RadioButton, Dataset::DATASET_SOURCE_RADIOML_2016_10A );
    datasetBtnGroup->addButton( mMainUi->DatasetRadioML2018RadioButton, Dataset::DATASET_SOURCE_RADIOML_2018_01 );
    datasetBtnGroup->addButton( mMainUi->DatasetHisarMod2019RadioButton, Dataset::DATASET_SOURCE_HISARMOD_2019_1 );

    connect( datasetBtnGroup, SIGNAL( idClicked(int) ), this, SLOT( updateDatasetSrc(int) ) );

    switch( mDatasetInstance->getSource() )
    {
        case Dataset::DATASET_SOURCE_RADIOML_2016_10A:
            mMainUi->DatasetRadioML2016RadioButton->setChecked( true );
            break;

        case Dataset::DATASET_SOURCE_RADIOML_2018_01:
            mMainUi->DatasetRadioML2018RadioButton->setChecked( true );
            break;

        case Dataset::DATASET_SOURCE_HISARMOD_2019_1:
            mMainUi->DatasetHisarMod2019RadioButton->setChecked( true );
            break;

        default:
            break;
    }

    connect( mMainUi->OpenDatasetButton, SIGNAL( clicked() ), this, SLOT( openDatasetSrc() ) );

    //*************************
    // parsers
    //*************************
    mPklParser->moveToThread( mPklParserThread );
    connect( mPklParserThread, SIGNAL( started() ), mPklParser, SLOT( parseDataset() ) );
    connect( mPklParser, SIGNAL( parseFinished() ), this, SLOT( handleDatasetParseFinished() ) );
    connect( mPklParser, SIGNAL( parseFinished() ), mPklParserThread, SLOT( quit() ) );

    mHdf5Parser->moveToThread( mHdf5ParserThread );
    connect( mHdf5ParserThread, SIGNAL( started() ), mHdf5Parser, SLOT( parseDatasetSingleModulation() ) );
    connect( mHdf5Parser, SIGNAL( parseFinished() ), this, SLOT( handleDatasetParseFinished() ) );
    connect( mHdf5Parser, SIGNAL( parseFinished() ), mHdf5ParserThread, SLOT( quit() ) );

    mCsvParser->moveToThread( mCsvParserThread );
    connect( mCsvParserThread, SIGNAL( started() ), mCsvParser, SLOT( parseDataset() ) );
    connect( mCsvParser, SIGNAL( parseFinished() ), this, SLOT( handleDatasetParseFinished() ) );
    connect( mCsvParser, SIGNAL( parseFinished() ), mCsvParserThread, SLOT( quit() ) );

    //*************************
    // modulation
    //*************************
    mModulationInstance = Modulation::getInstance();

    mMainUi->ModulationTypeValue->clear();
    mMainUi->ModulationFamilyValue->clear();

    connect( mMainUi->ModulationNameComboBox, QOverload<int>::of( &QComboBox::activated ), this, &RadioModTx::handleModulationNameChanged );
    connect( mMainUi->ModulationSnrComboBox, QOverload<int>::of( &QComboBox::activated ), this, &RadioModTx::handleModulationSnrChanged );

    //*************************
    // Tx
    //*************************
    mMainUi->StartFramesButton->setEnabled( false );
    mMainUi->StopFramesButton->setEnabled( false );

    mTxHalInstance = TxHal::getInstance();   
    updateTxList();

    connect( mMainUi->FramesTxComboBox, QOverload<int>::of( &QComboBox::activated ), this, &RadioModTx::handleTxChanged );

    connect( mMainUi->FloSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( handleChangedTxFlo(double) ) );   
    connect( mMainUi->NcoGainSpinBox, SIGNAL( valueChanged(double) ), this, SLOT( handleChangedTxNcoGain(double) ) );

    connect( mMainUi->StartFramesButton,  SIGNAL( clicked() ), this, SLOT( handleStartTxStreaming() ) );
    connect( mMainUi->StopFramesButton,  SIGNAL( clicked() ), this, SLOT( handleStopTxStreaming() ) );

    //*************************
    // status bar
    //*************************
    mMainUi->statusbar->showMessage( "Ready", 3000 );
}


//!************************************************************************
//! Destructor
//!************************************************************************
RadioModTx::~RadioModTx()
{
    delete mMainUi;
}


//!************************************************************************
//! Handle for changing the Tx LO frequency
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::handleChangedTxFlo
    (
    double aFrequencyMhz    //!< LO frequency [MHz]
    )
{
    if( mTxHalInstance->isInitialized() )
    {
        mTxHalInstance->setTxLoFrequency( aFrequencyMhz * 1.e6 );
    }
}


//!************************************************************************
//! Handle for changing the Tx NCO gain scale
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::handleChangedTxNcoGain
    (
    double aGainScale       //!< gain scale [0..1]
    )
{
    if( mTxHalInstance->isInitialized() )
    {
        mTxHalInstance->setTxNcoGainScale( aGainScale );
    }
}


//!************************************************************************
//! Handle for updates when parsing is finished
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::handleDatasetParseFinished()
{
    updateControlsParseFinished();

    switch( mDatasetType )
    {
        case Dataset::DATASET_SOURCE_RADIOML_2016_10A:
            mMap = mPklParser->getMap( mParserStatus );
            mUniqueModVec = mPklParser->getUniqueModVec();
            mUniqueSnrVec = mPklParser->getUniqueSnrVec();
            break;

        case Dataset::DATASET_SOURCE_RADIOML_2018_01:
            mMap = mHdf5Parser->getMap( mParserStatus );
            mUniqueModVec = Hdf5Parser::MODULATION_MAPPING;
            mUniqueSnrVec = mHdf5Parser->getUniqueSnrVec();
            break;

        case Dataset::DATASET_SOURCE_HISARMOD_2019_1:
            mMap = mCsvParser->getMap( mParserStatus );
            mUniqueModVec = mCsvParser->getUniqueModVec();
            mUniqueSnrVec = mCsvParser->getUniqueSnrVec();
            break;

        default:
            break;
    }

    mMainUi->statusbar->showMessage( mParserStatus ? "Parsing done." : "Parsing failed." );

    if( mParserStatus )
    {
        if( mTxHalInstance->isInitialized() )
        {
            mTxHalInstance->updateSamplingFrequency( mDatasetType );
        }

        updateModulationControls();
        updateSnrControls();        
    }

    updateTxControls();
}


//!************************************************************************
//! Handle for updates when modulation name changed
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::handleModulationNameChanged
    (
    int aIndex  //!< index
    )
{
    mCrtModulation = static_cast<Modulation::ModulationName>( mMainUi->ModulationNameComboBox->itemData( aIndex ).value<int>() );

    mMainUi->ModulationTypeValue->setText( QString::fromStdString( mModulationInstance->getTypeString( mCrtModulation ) ) );
    mMainUi->ModulationFamilyValue->setText( QString::fromStdString( mModulationInstance->getFamilyString( mCrtModulation ) ) );

    if( Dataset::DATASET_SOURCE_RADIOML_2018_01 == mDatasetType )
    {
        mMainUi->StartFramesButton->setEnabled( false );
        mMainUi->StopFramesButton->setEnabled( false );
    }
}


//!************************************************************************
//! Handle for updates when modulation SNR changed
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::handleModulationSnrChanged
    (
    int aIndex  //!< index
    )
{
    mCrtSnrDb = mMainUi->ModulationSnrComboBox->itemData( aIndex ).value<int>();
}


//!************************************************************************
//! Handle for starting the Tx stream
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::handleStartTxStreaming()
{
    if( mTxHalInstance->isInitialized() )
    {
        mMainUi->StartFramesButton->setEnabled( false );
        mMainUi->StopFramesButton->setEnabled( true );

        mMainUi->DatasetGroupBox->setEnabled( false );
        mMainUi->ModulationGroupBox->setEnabled( false );
        mMainUi->FramesTxComboBox->setEnabled( false );

        Dataset::ModulationSnrPair modSnrPair = std::make_pair( mCrtModulation, mCrtSnrDb );
        mTxHalInstance->getData( mMap.at( modSnrPair ) );

        std::string dfn = makeDumpFilename();
        mTxHalInstance->getDumpFilename( dfn );

        mTxHalInstance->startStreaming();
    }
}


//!************************************************************************
//! Handle for stopping the Tx stream
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::handleStopTxStreaming()
{
    if( mTxHalInstance->isInitialized() )
    {
        mMainUi->StartFramesButton->setEnabled( true );
        mMainUi->StopFramesButton->setEnabled( false );

        mMainUi->DatasetGroupBox->setEnabled( true );
        mMainUi->ModulationGroupBox->setEnabled( true );
        mMainUi->FramesTxComboBox->setEnabled( true );

        mTxHalInstance->stopStreaming();
    }
}


//!************************************************************************
//! Handle for updates when modulation SNR changed
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::handleTxChanged
    (
    int aIndex  //!< index
    )
{
    mTxIioScanIndex = mMainUi->FramesTxComboBox->itemData( aIndex ).value<int>();
    mTxHalInstance->initializeTxDevice( mTxIioScanIndex );
    updateTxControls();
}


//!************************************************************************
//! Make the filename for dumping data
//!
//! @returns The dump filename
//!************************************************************************
std::string RadioModTx::makeDumpFilename() const
{
    std::string dumpFilename;

    switch( mDatasetType )
    {
        case Dataset::DATASET_SOURCE_RADIOML_2016_10A:
            dumpFilename += "RadioML2016.10A";
            break;

        case Dataset::DATASET_SOURCE_RADIOML_2018_01:
            dumpFilename += "RadioML2018.01";
            break;

        case Dataset::DATASET_SOURCE_HISARMOD_2019_1:
            dumpFilename += "HisarMod2019.1";
            break;

        default:
            break;
    }

    dumpFilename += "_";
    dumpFilename += Modulation::MODULATION_NAME_ALIAS.at( mCrtModulation ).at( 0 );
    dumpFilename += "_";
    dumpFilename += std::to_string( mCrtSnrDb );
    dumpFilename += "dB";
    dumpFilename += ".txt";

    return dumpFilename;
}


//!************************************************************************
//! Open a dataset source
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::openDatasetSrc()
{
    QString filemask;
    QString selectedFilter;
    mDatasetType = mDatasetInstance->getSource();

    switch( mDatasetType )
    {
        case Dataset::DATASET_SOURCE_RADIOML_2016_10A:
            filemask = "Pickle files (*.pkl)";
            break;

        case Dataset::DATASET_SOURCE_RADIOML_2018_01:
            filemask = "HDF5 files (*.hdf5 *h5)";
            break;

        case Dataset::DATASET_SOURCE_HISARMOD_2019_1:
            filemask = "CSV files (*.csv)";
            break;

        default:
            break;
    }

    QString fileName = QFileDialog::getOpenFileName( this,
                                                     "Open dataset file",
                                                     "",
                                                     filemask,
                                                     &selectedFilter,
                                                     QFileDialog::DontUseNativeDialog
                                                    );

    std::string inputFilename = fileName.toStdString();
    std::ifstream inputFile( inputFilename );
    bool canOpen = inputFile.is_open();

    if( canOpen )
    {
        inputFile.close();        

        switch( mDatasetType )
        {
            case Dataset::DATASET_SOURCE_RADIOML_2016_10A:
                mPklParser->setFile( inputFilename );
                mPklParserThread->start();
                mMainUi->statusbar->showMessage( "Parsing pickle file, please wait... " );
                updateControlsParseStarted();
                break;

            case Dataset::DATASET_SOURCE_RADIOML_2018_01:
                mHdf5Parser->setSingleModulation( mCrtModulation );
                mHdf5Parser->setFile( inputFilename );
                mHdf5ParserThread->start();
                mMainUi->statusbar->showMessage( "Parsing HDF5 file, please wait... " );
                updateControlsParseStarted();
                break;

            case Dataset::DATASET_SOURCE_HISARMOD_2019_1:
                mCsvParser->setFile( inputFilename );
                mCsvParserThread->start();
                mMainUi->statusbar->showMessage( "Parsing CSV file, please wait... " );
                updateControlsParseStarted();
                break;

            default:
                break;
        }
    }
    else if( fileName.size() )
    {
        QMessageBox msgBox;
        msgBox.setText( "Can not open file \"" + fileName + "\"." );
        msgBox.exec();
    }    
}


//!************************************************************************
//! Update UI controls when parsing finished
//!
//! @returns nothing
//!************************************************************************
void RadioModTx::updateControlsParseFinished()
{
    mMainUi->DatasetGroupBox->setEnabled( true );
    mMainUi->ModulationGroupBox->setEnabled( true );
    mMainUi->FramesGroupBox->setEnabled( true );    
}


//!************************************************************************
//! Update UI controls when parsing started
//!
//! @returns nothing
//!************************************************************************
void RadioModTx::updateControlsParseStarted()
{
    mParserStatus = false;

    mMainUi->DatasetGroupBox->setEnabled( false );
    mMainUi->ModulationGroupBox->setEnabled( false );
    mMainUi->FramesGroupBox->setEnabled( false );

    mMainUi->ModulationNameComboBox->clear();
    mMainUi->ModulationSnrComboBox->clear();

    mMainUi->ModulationTypeValue->clear();
    mMainUi->ModulationFamilyValue->clear();
}


//!************************************************************************
//! Update the dataset source
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::updateDatasetSrc
    (
    int aIndex  //!< index
    )
{
    mParserStatus = false;
    mDatasetInstance->getSource() = static_cast<Dataset::DatasetSource>( aIndex );

    if( Dataset::DATASET_SOURCE_RADIOML_2018_01 == mDatasetInstance->getSource() )
    {
        mMainUi->ModulationNameComboBox->clear();
        mUniqueModVec = Hdf5Parser::MODULATION_MAPPING;

        for( size_t i = 0; i < mUniqueModVec.size(); i++ )
        {
            Modulation::ModulationName modName = mUniqueModVec.at( i );
            std::string modNameString = mModulationInstance->getModulationString( modName );
            mMainUi->ModulationNameComboBox->addItem( QString::fromStdString( modNameString ), QVariant::fromValue( static_cast<int>( modName ) ) );
        }

        mCrtModulation = static_cast<Modulation::ModulationName>( mMainUi->ModulationNameComboBox->itemData( 0 ).value<int>() );

        mMainUi->ModulationTypeValue->setText( QString::fromStdString( mModulationInstance->getTypeString( mCrtModulation ) ) );
        mMainUi->ModulationFamilyValue->setText( QString::fromStdString( mModulationInstance->getFamilyString( mCrtModulation ) ) );

        mMainUi->ModulationSnrComboBox->clear();
    }
    else
    {
        mMainUi->ModulationNameComboBox->clear();
        mUniqueModVec.clear();
        mCrtModulation = Modulation::NAME_UNKNOWN;

        mMainUi->ModulationTypeValue->clear();
        mMainUi->ModulationFamilyValue->clear();

        mMainUi->ModulationSnrComboBox->clear();
    }

    mMainUi->StartFramesButton->setEnabled( false );
    mMainUi->StopFramesButton->setEnabled( false );
}


//!************************************************************************
//! Update the modulation controls
//!
//! @returns nothing
//!************************************************************************
void RadioModTx::updateModulationControls()
{   
    for( size_t i = 0; i < mUniqueModVec.size(); i++ )
    {
        Modulation::ModulationName modName = mUniqueModVec.at( i );
        std::string modNameString = mModulationInstance->getModulationString( modName );
        mMainUi->ModulationNameComboBox->addItem( QString::fromStdString( modNameString ), QVariant::fromValue( static_cast<int>( modName ) ) );
    }

    if( Dataset::DATASET_SOURCE_RADIOML_2018_01 != mDatasetType )
    {
        mCrtModulation = static_cast<Modulation::ModulationName>( mMainUi->ModulationNameComboBox->itemData( 0 ).value<int>() );
    }
    else
    {
        int crtIndex = mMainUi->ModulationNameComboBox->findData( mCrtModulation );
        mMainUi->ModulationNameComboBox->setCurrentIndex( crtIndex );
    }

    mMainUi->ModulationTypeValue->setText( QString::fromStdString( mModulationInstance->getTypeString( mCrtModulation ) ) );
    mMainUi->ModulationFamilyValue->setText( QString::fromStdString( mModulationInstance->getFamilyString( mCrtModulation ) ) );
}


//!************************************************************************
//! Update the SNR controls
//!
//! @returns nothing
//!************************************************************************
void RadioModTx::updateSnrControls()
{
    for( size_t i = 0; i < mUniqueSnrVec.size(); i++ )
    {
        int snrDb = mUniqueSnrVec.at( i );
        mMainUi->ModulationSnrComboBox->addItem( QString::number( snrDb ) + " dB", QVariant::fromValue( snrDb ) );
    }

    mCrtSnrDb = mMainUi->ModulationSnrComboBox->itemData( 0 ).value<int>();
}


//!************************************************************************
//! Update the Tx controls
//!
//! @returns nothing
//!************************************************************************
void RadioModTx::updateTxControls()
{
    bool isInit = mTxHalInstance->isInitialized();

    mMainUi->FloSpinBox->setEnabled( isInit );

    // LO frequency
    AdiTrx::IntegerRange txFloRangeHz = mTxHalInstance->getTxLoFrequencyRange();
    mMainUi->FloSpinBox->setMinimum( txFloRangeHz.min / 1.e6 );
    mMainUi->FloSpinBox->setMaximum( txFloRangeHz.max / 1.e6 );
    mMainUi->FloSpinBox->setSingleStep( 1 ); // 1 MHz

    int64_t frequencyHz = 0;
    mTxHalInstance->getTxLoFrequency( frequencyHz );
    mMainUi->FloSpinBox->setValue( frequencyHz / 1.e6 );

    // NCO gain
    if( TxHal::TX_DEVICE_AD9081 == mTxHalInstance->getTxDevice() )
    {
        mMainUi->NcoGainLabel->setVisible( true );
        mMainUi->NcoGainSpinBox->setVisible( true );

        mMainUi->NcoGainLabel->setEnabled( isInit );
        mMainUi->NcoGainSpinBox->setEnabled( isInit );

        double ncoGainScale = 0;

        if( isInit )
        {
            mTxHalInstance->getTxNcoGainScale( ncoGainScale );
        }

        mMainUi->NcoGainSpinBox->setValue( ncoGainScale );
    }
    else
    {
        mMainUi->NcoGainLabel->setVisible( false );
        mMainUi->NcoGainSpinBox->setVisible( false );
    }

    // sampling frequency
    mTxHalInstance->getTxSamplingFrequency( frequencyHz );
    mMainUi->FsampValue->setText( QString::number( frequencyHz / 1.e6, 'f', 3 ) + " MHz" );

    // BW
    int64_t bwHz = 0;
    mTxHalInstance->getTxBandwidth( bwHz );
    mMainUi->BwValue->setText( QString::number( bwHz / 1.e6, 'f', 3 ) + " MHz" );

    // Gain
    double gainDb = 0;
    mTxHalInstance->getTxHwGain( gainDb );
    mMainUi->GainValue->setText( QString::number( gainDb, 'f', 2 ) + " dB" );

    // buttons
    mMainUi->StartFramesButton->setEnabled( mParserStatus && isInit );
    mMainUi->StopFramesButton->setEnabled( false );
}


//!************************************************************************
//! Update the list of Tx contexts
//!
//! @returns nothing
//!************************************************************************
/* slot */ void RadioModTx::updateTxList()
{
    mTxHalInstance->updateIioScanContexts();

    std::vector<TxHal::IioScanContext> iscVec = mTxHalInstance->getIioScanContexts();
    mMainUi->FramesTxComboBox->clear();
    size_t iscLen = iscVec.size();

    for( size_t i = 0; i < iscLen; i++ )
    {
        mMainUi->FramesTxComboBox->addItem( QString::fromStdString( iscVec.at( i ).uri ), QVariant::fromValue( i ) );
        mMainUi->FramesTxComboBox->setItemData( i, QString::fromStdString( iscVec.at( i ).description ), Qt::ToolTipRole );
    }

    mTxIioScanIndex = iscLen ? 0 : -1;
    mTxHalInstance->initializeTxDevice( mTxIioScanIndex );
    updateTxControls();
}
