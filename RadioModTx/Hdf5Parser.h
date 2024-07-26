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
Hdf5Parser.h

This file contains the definitions for HDF5 parser.
*/

#ifndef Hdf5Parser_h
#define Hdf5Parser_h

#include "DatasetParser.h"

#include "hdf5.h"

#include <memory>
#include <vector>

#include <QVariant>


//************************************************************************
// Class for handling HDF5 tree items
//************************************************************************
class Hdf5TreeItem
{
    //************************************************************************
    // functions
    //************************************************************************
    public:
        explicit Hdf5TreeItem
            (
            QVariant        aData,                  //!< item data
            Hdf5TreeItem*   aParentItem = nullptr   //!< parent node
            );

        void appendChild
            (
            std::unique_ptr<Hdf5TreeItem>&& aChild  //!< pointer to child item
            );

        Hdf5TreeItem* getChild
            (
            int aRow    //!< row number
            );

        int getChildCount() const;

        QVariant getData() const;

    //************************************************************************
    // variables
    //************************************************************************
    private:
        std::vector<std::unique_ptr<Hdf5TreeItem>>  mChildItems;    //!< child items vector
        QVariant                                    mItemData;      //!< item data
        Hdf5TreeItem*                               mParentItem;    //!< parent node
};


//************************************************************************
// Class for handling HDF5 datasets
//************************************************************************
class Hdf5Dataset
{
    //************************************************************************
    // functions
    //************************************************************************
    public:
        Hdf5Dataset
            (
            const char*                 aPath,          //!< file path
            const std::vector<hsize_t>& aDimensionsVec, //!< dimensions vector
            size_t                      aDatatypeSize,  //!< datatype size
            H5T_sign_t                  aDatatypeSign,  //!< datatype sign
            H5T_class_t                 aDatatypeClass  //!< datatype class
            );

        ~Hdf5Dataset();

        void store
            (
            void* aBuffer   //!< data buffer
            );

    //************************************************************************
    // variables
    //************************************************************************
    public:
        void*                   mDataBuffer;        //!< buffer where data is stored
        std::string             mFilePath;          //!< file path
        std::vector<hsize_t>    mDimensionsVec;     //!< dimensions vector
        size_t                  mDatatypeSize;      //!< datatype size
        H5T_sign_t              mDatatypeSign;      //!< datatype sign
        H5T_class_t             mDatatypeClass;     //!< datatype class
};


//************************************************************************
// Class for handling HDF5 item data
//************************************************************************
class Hdf5ItemData
{
    //************************************************************************
    // constants and types
    //************************************************************************
    public:
        typedef enum
        {
            ITEM_TYPE_ROOT,
            ITEM_TYPE_GROUP,
            ITEM_TYPE_VARIABLE,
            ITEM_TYPE_ATTRIBUTE
        }ItemType;

    //************************************************************************
    // functions
    //************************************************************************
    public:
        Hdf5ItemData
            (
            ItemType            aItemType,  //!< item type
            const std::string&  aFileName,  //!< filename
            const std::string&  aItemName,  //!< item name
            Hdf5Dataset*        aDataset    //!< dataset
            );

        ~Hdf5ItemData();

    //************************************************************************
    // variables
    //************************************************************************
    public:
        std::string     mFileName;    //!< file name
        std::string     mItemName;    //!< item name
        ItemType        mItemType;    //!< item type
        Hdf5Dataset*    mDataset;     //!< variable data
};


Q_DECLARE_METATYPE( Hdf5ItemData* )

//************************************************************************
// Class for handling a HDF5 visit
//************************************************************************
class Hdf5Visit
{
    //************************************************************************
    // constants and types
    //************************************************************************
    public:
        typedef struct
        {
            H5O_info_t      objInfo;    //!< HDF5 object information
            size_t          size;       //!< size
        }ObjInfoSize;

    //************************************************************************
    // functions
    //************************************************************************
    public:
        Hdf5Visit();

        int visit
            (
            const char* aFileName   //!< filename
            );

    private:
        static herr_t visitLinkCallback
            (
            hid_t               aLocationId,    //!< location ID
            const char*         aName,          //!< name
            const H5L_info_t*   aLinkInfo,      //!< link information
            void*               aData           //!< data
            );

    //************************************************************************
    // variables
    //************************************************************************
    public:
        std::vector<ObjInfoSize> mVisitInfo;     //!< visit information
};


//************************************************************************
// Class for handling the Hierarchical Data Format (HDF5) 5 parser (*.hdf5, *.h5)
//************************************************************************
class Hdf5Parser : public DatasetParser
{
    Q_OBJECT
    //************************************************************************
    // constants and types
    //************************************************************************
    public:
        static const std::vector<Modulation::ModulationName> MODULATION_MAPPING;

    private:
        typedef struct
        {
            char*   name;       //!< HDF5 object name
            int     type;       //!< HDF5 object type
        }Hdf5NameType;

    //************************************************************************
    // functions
    //************************************************************************
    public:
        Hdf5Parser();

    public slots:
        void parseDataset();

        void parseDatasetSingleModulation();

    private:
        static herr_t countObjectsCallback
            (
            hid_t               aLocationId,    //!< location ID
            const char*         aName,          //!< name
            const H5L_info_t*   aLinkInfo,      //!< link information
            void*               aData           //!< data
            );

        Hdf5Visit::ObjInfoSize* findObject
            (
            haddr_t             aAddress        //!< object address
            );

        static herr_t getNameTypeCallback
            (
            hid_t               aLocationId,    //!< location ID
            const char*         aName,          //!< name
            const H5L_info_t*   aLinkInfo,      //!< link information
            void*               aData           //!< data
            );

        Hdf5ItemData* getTreeItemData
            (
            Hdf5TreeItem*       aTreeItem       //!< item in tree
            );

        int iterateGroup
            (
            const std::string&  aFileName,      //!< filename
            const std::string&  aGroupPath,     //!< group path
            const hid_t         aLocationId,    //!< location ID
            Hdf5TreeItem*       aTreeItemParent //!< parent item in tree
            );

        bool loadTreeItem
            (
            Hdf5TreeItem*       aTreeItem       //!< item in tree
            );

    signals:
        void parseFinished();

    //************************************************************************
    // variables
    //************************************************************************
    private:
        Hdf5Visit       mVisit;     //!< HDF5 visit
        Hdf5TreeItem*   mRootItem;  //!< root item
};

#endif // Hdf5Parser_h
