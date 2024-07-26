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
Hdf5Parser.cpp

This file contains the sources for HDF5 parser.
*/

#include "Hdf5Parser.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <new>
#include <utility>


//!************************************************************************
//! Constructor
//!************************************************************************
Hdf5TreeItem::Hdf5TreeItem
    (
    QVariant        aData,          //!< item data
    Hdf5TreeItem*   aParentItem     //!< parent node
    )
    : mItemData( std::move( aData ) )
    , mParentItem( aParentItem )
{
}


//!************************************************************************
//! Appends a child to current node
//!
//! @returns nothing
//!************************************************************************
void Hdf5TreeItem::appendChild
    (
    std::unique_ptr<Hdf5TreeItem>&& aChild      //!< pointer to child item
    )
{
    mChildItems.push_back( std::move( aChild ) );
}


//!************************************************************************
//! Get the child placed at a given row
//!
//! @returns The child item
//!************************************************************************
Hdf5TreeItem* Hdf5TreeItem::getChild
    (
    int aRow    //!< row number
    )
{
    return aRow >= 0 && aRow < getChildCount() ? mChildItems.at( aRow ).get() : nullptr;
}


//!************************************************************************
//! Get the child count
//!
//! @returns The child count
//!************************************************************************
int Hdf5TreeItem::getChildCount() const
{
    return static_cast<int>( mChildItems.size() );
}


//!************************************************************************
//! Get the data of a node
//!
//! @returns The data of a node
//!************************************************************************
QVariant Hdf5TreeItem::getData() const
{
    return mItemData;
}


//!************************************************************************
//! Constructor
//!************************************************************************
Hdf5Dataset::Hdf5Dataset
    (
    const char*                 aPath,          //!< file path
    const std::vector<hsize_t>& aDimensionsVec, //!< dimensions vector
    size_t                      aDatatypeSize,  //!< datatype size
    H5T_sign_t                  aDatatypeSign,  //!< datatype sign
    H5T_class_t                 aDatatypeClass  //!< datatype class
    )
    : mDataBuffer( nullptr )
    , mFilePath( aPath )
    , mDimensionsVec( aDimensionsVec )
    , mDatatypeSize( aDatatypeSize )
    , mDatatypeSign( aDatatypeSign )
    , mDatatypeClass( aDatatypeClass )
{
}


//!************************************************************************
//! Destructor
//!************************************************************************
Hdf5Dataset::~Hdf5Dataset()
{
    free( mDataBuffer );
}


//!************************************************************************
//! Store the data buffer
//!
//! @returns nothing
//!************************************************************************
void Hdf5Dataset::store
    (
    void* aBuffer   //!< data buffer
    )
{
    mDataBuffer = aBuffer;
}


//!************************************************************************
//! Constructor
//!************************************************************************
Hdf5ItemData::Hdf5ItemData
    (
    ItemType            aItemType,  //!< item type
    const std::string&  aFileName,  //!< filename
    const std::string&  aItemName,  //!< item name
    Hdf5Dataset*        aDataset    //!< dataset
    )
    : mFileName( aFileName )
    , mItemName( aItemName )
    , mItemType( aItemType )
    , mDataset( aDataset )
{
}


//!************************************************************************
//! Destructor
//!************************************************************************
Hdf5ItemData::~Hdf5ItemData()
{
    delete mDataset;
}


//!************************************************************************
//! Constructor
//!************************************************************************
Hdf5Visit::Hdf5Visit()
{
}


//!************************************************************************
//! HDF5 visit
//!
//! @returns 0 at success, -1 otherwise
//!************************************************************************
int Hdf5Visit::visit
    (
    const char* aFileName   //!< filename
    )
{
    int retVal = 0;
    hid_t fileId;

    if( ( fileId = H5Fopen( aFileName, H5F_ACC_RDONLY, H5P_DEFAULT ) ) >= 0 )
    {
        Hdf5Visit* data = this;

        if( H5Lvisit_by_name( fileId, "/", H5_INDEX_NAME, H5_ITER_INC, visitLinkCallback, data, H5P_DEFAULT ) >= 0 )
        {
            H5Fclose( fileId );
        }
        else
        {
            retVal = -1;
        }
    }
    else
    {
        retVal = -1;
    }

    return retVal;
}


//!************************************************************************
//! Callback function for HDF5 visit
//!
//! @returns The iteration status
//!************************************************************************
herr_t Hdf5Visit::visitLinkCallback
    (
    hid_t               aLocationId,    //!< location ID
    const char*         aName,          //!< name
    const H5L_info_t*   aLinkInfo,      //!< link information
    void*               aData           //!< data
    )
{
    herr_t retVal = H5_ITER_CONT;
    Hdf5Visit* data = static_cast<Hdf5Visit*>( aData );

    if( H5L_TYPE_HARD == aLinkInfo->type )
    {
        H5O_info_t objInfo;

        if( H5Oget_info_by_name( aLocationId, aName, &objInfo, H5P_DEFAULT ) >= 0 )
        {
            ObjInfoSize objInfoSize;
            objInfoSize.objInfo = objInfo;
            objInfoSize.size = 0;
            data->mVisitInfo.push_back( objInfoSize );
        }
        else
        {
            retVal = H5_ITER_ERROR;
        }
    }

    return retVal;
}


const std::vector<Modulation::ModulationName> Hdf5Parser::MODULATION_MAPPING =
{
    // ASK
    Modulation::NAME_OOK,
    Modulation::NAME_4ASK,
    Modulation::NAME_8ASK,
    // PSK
    Modulation::NAME_BPSK,
    Modulation::NAME_QPSK,
    Modulation::NAME_8PSK,
    Modulation::NAME_16PSK,
    Modulation::NAME_32PSK,
    // APSK
    Modulation::NAME_16APSK,
    Modulation::NAME_32APSK,
    Modulation::NAME_64APSK,
    Modulation::NAME_128APSK,
    // QAM
    Modulation::NAME_16QAM,
    Modulation::NAME_32QAM,
    Modulation::NAME_64QAM,
    Modulation::NAME_128QAM,
    Modulation::NAME_256QAM,
    // AM
    Modulation::NAME_AM_SSB_WC,
    Modulation::NAME_AM_SSB_SC,
    Modulation::NAME_AM_DSB_WC,
    Modulation::NAME_AM_DSB_SC,
    // FM
    Modulation::NAME_FM,
    // FSK
    Modulation::NAME_GMSK,
    // PSK
    Modulation::NAME_OQPSK
};

//!************************************************************************
//! Constructor
//!************************************************************************
Hdf5Parser::Hdf5Parser()
{
}


//!************************************************************************
//! Callback function for H5Literate counting objects
//!
//! @returns Iteration continue
//!************************************************************************
herr_t Hdf5Parser::countObjectsCallback
    (
    hid_t               aLocationId,    //!< location ID
    const char*         aName,          //!< name
    const H5L_info_t*   aLinkInfo,      //!< link information
    void*               aData           //!< data
    )
{
    Q_UNUSED( aLocationId );
    Q_UNUSED( aName );
    Q_UNUSED( aLinkInfo );
    hsize_t* data = static_cast<hsize_t*>( aData );
    ( *data )++;

    return H5_ITER_CONT;
}


//!************************************************************************
//! Find an object
//!
//! @returns The address of the information and size object
//!************************************************************************
Hdf5Visit::ObjInfoSize* Hdf5Parser::findObject
    (
    haddr_t aAddress    //!< object address
    )
{
    for( size_t i = 0; i < mVisit.mVisitInfo.size(); i++ )
    {
        if( aAddress == mVisit.mVisitInfo.at(i).objInfo.addr )
        {
            return &( mVisit.mVisitInfo.at(i) );
        }
    }

    return nullptr;
}


//!************************************************************************
//! Callback function for getting the type and name of an object
//!
//! @returns Iteration stop callback value
//!************************************************************************
herr_t Hdf5Parser::getNameTypeCallback
    (
    hid_t               aLocationId,    //!< location ID
    const char*         aName,          //!< name
    const H5L_info_t*   aLinkInfo,      //!< link information
    void*               aData           //!< data
    )
{
    Q_UNUSED( aLinkInfo );
    H5G_stat_t objInfo;
    H5Gget_objinfo( aLocationId, aName, 0, &objInfo );

    ( ( Hdf5NameType* )aData )->type = objInfo.type;
    ( ( Hdf5NameType* )aData )->name = static_cast<char*>( strdup( aName ) );

    return H5_ITER_STOP;
}


//!************************************************************************
//! Get the data of a HDF5 tree item
//!
//! @returns The address of the data
//!************************************************************************
Hdf5ItemData* Hdf5Parser::getTreeItemData
    (
    Hdf5TreeItem* aTreeItem     //!< tree item
    )
{
    QVariant qvData = aTreeItem->getData();
    Hdf5ItemData* itemData = qvData.value<Hdf5ItemData*>();
    return itemData;
}


//!************************************************************************
//! Iterate in a HDF5 group
//!
//! @returns 0
//!************************************************************************
int Hdf5Parser::iterateGroup
    (
    const std::string&  aFileName,      //!< filename
    const std::string&  aGroupPath,     //!< group path
    const hid_t         aLocationId,    //!< location ID
    Hdf5TreeItem*       aTreeItemParent //!< parent item
    )
{
    hsize_t objCount = 0;
    H5Literate( aLocationId, H5_INDEX_NAME, H5_ITER_INC, nullptr, countObjectsCallback, &objCount );

    for( hsize_t objIndex = 0; objIndex < objCount; objIndex++ )
    {
        hsize_t crtIndex = objIndex;
        Hdf5NameType info;
        H5Literate( aLocationId, H5_INDEX_NAME, H5_ITER_INC, &crtIndex, getNameTypeCallback, &info );

        std::string path = aGroupPath;

        if( aGroupPath != "/" )
        {
            path += "/";
        }

        path += info.name;

        if( H5G_DATASET == info.type )
        {
            hid_t datasetId = H5Dopen2( aLocationId, info.name, H5P_DEFAULT );
            hid_t spaceId = H5Dget_space( datasetId );

            hsize_t dimensionsSize[H5S_MAX_RANK];
            int dimensionsCount = H5Sget_simple_extent_dims( spaceId, dimensionsSize, nullptr);

            hid_t datatypeId = H5Dget_type( datasetId );
            hid_t nativeDatatypeId = H5Tget_native_type( datatypeId, H5T_DIR_DEFAULT );

            size_t nativeDatatypeSize = H5Tget_size( nativeDatatypeId );
            H5T_sign_t nativeDatatypeSign = H5Tget_sign( nativeDatatypeId );
            H5T_class_t nativeDatatypeClass = H5Tget_class( nativeDatatypeId );

            H5Sclose( spaceId );
            H5Tclose( datatypeId );
            H5Tclose( nativeDatatypeId );

            std::vector<hsize_t> dimensionsVec;

            for( int j = 0; j < dimensionsCount; j++ )
            {
                dimensionsVec.push_back( dimensionsSize[j] );
            }

            Hdf5Dataset* dataset = new Hdf5Dataset( path.c_str(),
                                                    dimensionsVec,
                                                    nativeDatatypeSize,
                                                    nativeDatatypeSign,
                                                    nativeDatatypeClass );

            Hdf5ItemData* itemData = new Hdf5ItemData( Hdf5ItemData::ITEM_TYPE_VARIABLE, aFileName, info.name, dataset );

            if( itemData )
            {
                QVariant qvData;
                qvData.setValue( itemData );
                Hdf5TreeItem* itemVariable = new Hdf5TreeItem( qvData, aTreeItemParent );

                mRootItem->appendChild( std::unique_ptr<Hdf5TreeItem>( itemVariable ) );
            }

            H5Dclose( datasetId );
        }
    }

    return 0;
}


//!************************************************************************
//! Load a HDF5 tree item
//!
//! @returns true at success
//!************************************************************************
bool Hdf5Parser::loadTreeItem
    (
    Hdf5TreeItem* aTreeItem     //!< tree item
    )
{
    Hdf5ItemData* itemData = getTreeItemData( aTreeItem );
    bool status = ( Hdf5ItemData::ITEM_TYPE_VARIABLE == itemData->mItemType );

    if( status )
    {
        status = ( nullptr == itemData->mDataset->mDataBuffer );
    }

    if( status )
    {
        hid_t fileId = H5Fopen( itemData->mFileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT );
        hid_t datasetId = H5Dopen2( fileId, itemData->mDataset->mFilePath.c_str(), H5P_DEFAULT );
        hid_t spaceId = H5Dget_space( datasetId );
        hid_t datatypeId = H5Dget_type( datasetId );
        hid_t nativeDatatypeId = H5Tget_native_type( datatypeId, H5T_DIR_DEFAULT );
        hsize_t nrOfElements = 1;

        for( size_t j = 0; j < itemData->mDataset->mDimensionsVec.size(); j++ )
        {
            nrOfElements *= itemData->mDataset->mDimensionsVec.at( j );
        }
        // nrOfElements = 5234491392 = 2555904 * 1024 * 2

        // 20937965568 = 4 * 5234491392 (approx. 19.5 GB)
        const size_t TOTAL_BYTES = itemData->mDataset->mDatatypeSize * nrOfElements;

        status = ( H5T_FLOAT == itemData->mDataset->mDatatypeClass );

        if( status )
        {
            itemData->mDataset->mDataBuffer = malloc( TOTAL_BYTES );

            status = ( nullptr != itemData->mDataset->mDataBuffer );

            if( !status )
            {
                std::cout << "Could not allocate " << TOTAL_BYTES / 1048576.0 << " MB. Not enough memory." << std::endl;
            }
        }

        if( status )
        {
            status = ( H5Dread( datasetId, nativeDatatypeId, H5S_ALL, H5S_ALL, H5P_DEFAULT, itemData->mDataset->mDataBuffer ) >= 0 );
        }

        if( status )
        {
            float* fltBuf = static_cast<float*>( itemData->mDataset->mDataBuffer );

            int crtSnrDb = -100;
            int oldSnrDb = crtSnrDb;

            mUniqueModVec.push_back( mSingleModulation );

            Dataset::ModulationSnrPair modSnrPair;

            Dataset::SignalData signalData;
            Dataset::FrameData frameData;
            Dataset::IQPoint iqPoint = { 0, 0 };

            // 218103808 = 5234491392 / 24
            const hsize_t NR_ELEMENTS_PER_MOD = nrOfElements / Dataset::MODULATIONS_NR.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 );

            // 2048 = 2 * 1024
            const hsize_t NR_ELEMENTS_PER_FRAME = 2 * Dataset::FRAME_LENGTH.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 );

            // 8388608 = 218103808 / 26
            const hsize_t NR_ELEMENTS_PER_MOD_SNR = NR_ELEMENTS_PER_MOD / Dataset::SNRS_NR.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 );;

            size_t modOffset = 0;

            for( size_t i = 0; i < MODULATION_MAPPING.size(); i++ )
            {
                if( MODULATION_MAPPING.at( i ) == mSingleModulation )
                {
                    modOffset = i;
                    break;
                }
            }

            const hsize_t START_ELEMENT = modOffset * NR_ELEMENTS_PER_MOD;
            const hsize_t STOP_ELEMENT = START_ELEMENT + NR_ELEMENTS_PER_MOD;

            for( hsize_t i = START_ELEMENT; i < STOP_ELEMENT; i += 2 )
            {
                crtSnrDb = -20 + 2 * ( ( i % NR_ELEMENTS_PER_MOD ) / NR_ELEMENTS_PER_MOD_SNR );

                if( crtSnrDb != oldSnrDb )
                {                    
                    mUniqueSnrVec.push_back( crtSnrDb );

                    modSnrPair = std::make_pair( mSingleModulation, crtSnrDb );

                    signalData.frameDataVec.clear();
                    signalData.maxVal = 0;
                }

                if( 0 == i % NR_ELEMENTS_PER_FRAME )
                {
                    frameData.clear();
                }

                hsize_t j = i;

                for( ; j < i + NR_ELEMENTS_PER_FRAME; j += 2 )
                {
                    float iPart = static_cast<float>( fltBuf[j] );
                    float qPart = static_cast<float>( fltBuf[j + 1] );
                    iqPoint = { iPart, qPart };
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

                if( Dataset::FRAME_LENGTH.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 ) != frameData.size() )
                {
                    status = false;
                    break;
                }

                signalData.frameDataVec.push_back( frameData );
                i = j - 2;

                if( i
               && ( 0 == ( i + 2 ) % NR_ELEMENTS_PER_MOD_SNR )
                  )
                {
                    if( Dataset::FRAMES_PER_MOD_SNR_NR.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 ) != signalData.frameDataVec.size() )
                    {
                        status = false;
                        break;
                    }

                    mMap.insert( std::pair<Dataset::ModulationSnrPair, Dataset::SignalData>( modSnrPair, signalData ) );
                }

                oldSnrDb = crtSnrDb;
            }

            free( fltBuf );
        }

        H5Dclose( datasetId );
        H5Sclose( spaceId );
        H5Tclose( datatypeId );
        H5Tclose( nativeDatatypeId );
        H5Fclose( fileId );
    }

    return status;
}


//!************************************************************************
//! Parse a dataset
//!
//! @returns nothing
//!************************************************************************
/* slot */ void Hdf5Parser::parseDataset()
{
}


//!************************************************************************
//! Parse a HDF5 file using the RadioML 2018.01 dataset syntax.
//! Only one modulation is selected from the entire dataset.
//!
//! @returns nothing
//!************************************************************************
/* slot */ void Hdf5Parser::parseDatasetSingleModulation()
{
    mUniqueModVec.clear();
    mUniqueSnrVec.clear();
    mMap.clear();

    bool parseFailed = ( Modulation::NAME_UNKNOWN == mSingleModulation );

    if( !parseFailed )
    {
        parseFailed = ( mVisit.visit( mFileName.c_str() ) < 0 );
    }

    if( !parseFailed )
    {
        hid_t fileId;

        if( ( fileId = H5Fopen( mFileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT ) ) >= 0 )
        {
            Hdf5ItemData* rootItemData = new Hdf5ItemData( Hdf5ItemData::ITEM_TYPE_GROUP, mFileName, "/", static_cast<Hdf5Dataset*>( nullptr ) );

            QVariant qvData;
            qvData.setValue( rootItemData );
            mRootItem = new Hdf5TreeItem( qvData );

            iterateGroup( mFileName, "/", fileId, mRootItem );

            H5Fclose( fileId );
        }
        else
        {
            parseFailed = true;
        }
    }

    if( !parseFailed )
    {
        // 2555904 = 4096 * 24 * 26
        const size_t ROWS_EXPECTED_NR = Dataset::FRAMES_PER_MOD_SNR_NR.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 )
                                      * Dataset::MODULATIONS_NR.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 )
                                      * Dataset::SNRS_NR.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 );

        bool foundX = false;
        bool foundY = false;
        bool foundZ = false;

        for( int i = 0; i < mRootItem->getChildCount(); i++ )
        {
            Hdf5TreeItem* crtChild = mRootItem->getChild( i );
            QVariant qvData = crtChild->getData();
            Hdf5ItemData* crtChildData = qvData.value<Hdf5ItemData*>();
            std::string childName = crtChildData->mItemName;

            if( "X" == childName ) // 3D of IQ values
            {
                if( 3 == crtChildData->mDataset->mDimensionsVec.size() )
                {
                    if( crtChildData->mDataset->mDimensionsVec.at( 0 ) != ROWS_EXPECTED_NR
                     || crtChildData->mDataset->mDimensionsVec.at( 1 ) != Dataset::FRAME_LENGTH.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 )
                     || crtChildData->mDataset->mDimensionsVec.at( 2 ) != 2
                      )
                    {
                        parseFailed = true;
                    }
                }
                else
                {
                    parseFailed = true;
                }

                if( parseFailed )
                {
                    break;
                }

                parseFailed = !loadTreeItem( crtChild );
                foundX = !parseFailed;
            }
            else if( "Y" == childName ) // matrix of modulations
            {
                if( 2 == crtChildData->mDataset->mDimensionsVec.size() )
                {
                    if( crtChildData->mDataset->mDimensionsVec.at( 0 ) != ROWS_EXPECTED_NR
                     || crtChildData->mDataset->mDimensionsVec.at( 1 ) != Dataset::MODULATIONS_NR.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 )
                      )
                    {
                        parseFailed = true;
                    }
                }
                else
                {
                    parseFailed = true;
                }

                if( parseFailed )
                {
                    break;
                }

                foundY = !parseFailed;
            }
            else if( "Z" == childName )  // array of SNRs
            {
                if( 2 == crtChildData->mDataset->mDimensionsVec.size() )
                {
                    if( crtChildData->mDataset->mDimensionsVec.at( 0 ) != ROWS_EXPECTED_NR
                     || crtChildData->mDataset->mDimensionsVec.at( 1 ) != 1
                      )
                    {
                        parseFailed = true;
                    }
                }
                else
                {
                    parseFailed = true;
                }

                if( parseFailed )
                {
                    break;
                }

                foundZ = !parseFailed;
            }
        }

        parseFailed = parseFailed || !foundX || !foundY || !foundZ;
    }

    if( !parseFailed )
    {
        removeDuplicates( mUniqueSnrVec );

        if( Dataset::SNRS_NR.at( Dataset::DATASET_SOURCE_RADIOML_2018_01 ) != mUniqueSnrVec.size() )
        {
            parseFailed = true;
        }
    }

    mStatus = !parseFailed;
    emit parseFinished();
}
