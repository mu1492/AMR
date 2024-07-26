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
Modulation.cpp

This file contains the sources for modulation.
*/

#include "Modulation.h"

#include <iostream>
#include <iterator>

#include <QMessageBox>


Modulation* Modulation::sInstance = nullptr;

const std::map<Modulation::ModulationName, std::vector<std::string>> Modulation::MODULATION_NAME_ALIAS =
{
    { Modulation::NAME_UNKNOWN,     { "" } },

    //*********
    // analog
    //*********
    // AM
    { Modulation::NAME_AM_SSB,      { "AM-SSB" } },
    { Modulation::NAME_AM_SSB_WC,   { "AM-SSB WC" } },
    { Modulation::NAME_AM_SSB_SC,   { "AM-SSB SC" } },
    { Modulation::NAME_AM_DSB,      { "AM-DSB" } },
    { Modulation::NAME_AM_DSB_WC,   { "AM-DSB WC" } },
    { Modulation::NAME_AM_DSB_SC,   { "AM-DSB SC" } },
    { Modulation::NAME_AM_USB,      { "AM-USB" } },
    { Modulation::NAME_AM_LSB,      { "AM-LSB" } },
    // FM
    { Modulation::NAME_FM,          { "FM" } },
    { Modulation::NAME_WBFM,        { "WBFM" } },
    // PM
    { Modulation::NAME_PM,          { "PM" } },

    //*********
    // digital
    //*********
    // APSK - Amplitude and Phase-Shift Keying
    { Modulation::NAME_16APSK,      { "16APSK", "APSK16" } },
    { Modulation::NAME_32APSK,      { "32APSK", "APSK32" } },
    { Modulation::NAME_64APSK,      { "64APSK", "APSK64" } },
    { Modulation::NAME_128APSK,     { "128APSK", "APSK128" } },
    // ASK - Amplitude-Shift Keying
    { Modulation::NAME_OOK,         { "OOK", "2ASK", "ASK2" } },
    { Modulation::NAME_4ASK,        { "4ASK", "ASK4" } },
    { Modulation::NAME_8ASK,        { "8ASK", "ASK8" } },
    // FSK - Frequency-Shift Keying
    { Modulation::NAME_2FSK,        { "2FSK", "FSK2" } },
    { Modulation::NAME_4FSK,        { "4FSK", "FSK4" } },
    { Modulation::NAME_8FSK,        { "8FSK", "FSK8" } },
    { Modulation::NAME_16FSK,       { "16FSK", "FSK16" } },
    { Modulation::NAME_GFSK,        { "GFSK" } },
    { Modulation::NAME_CPFSK,       { "CPFSK" } },
    { Modulation::NAME_GMSK,        { "GMSK" } },
    // PSK - Phase-Shift Keying
    { Modulation::NAME_BPSK,        { "BPSK", "2PSK", "PSK2" } },
    { Modulation::NAME_QPSK,        { "QPSK", "4PSK", "PSK4" } },
    { Modulation::NAME_8PSK,        { "8PSK", "PSK8" } },
    { Modulation::NAME_16PSK,       { "16PSK", "PSK16" } },
    { Modulation::NAME_32PSK,       { "32PSK", "PSK32" } },
    { Modulation::NAME_64PSK,       { "64PSK", "PSK64" } },
    { Modulation::NAME_OQPSK,       { "OQPSK" } },
    // PAM - Pulse-Amplitude Modulation
    { Modulation::NAME_4PAM,        { "4PAM", "PAM4" } },
    { Modulation::NAME_8PAM,        { "8PAM", "PAM8" } },
    { Modulation::NAME_16PAM,       { "16PAM", "PAM16" } },
    // QAM - Quadrature Amplitude Modulation
    { Modulation::NAME_4QAM,        { "4QAM", "QAM4" } },
    { Modulation::NAME_8QAM,        { "8QAM", "QAM8" } },
    { Modulation::NAME_16QAM,       { "16QAM", "QAM16" } },
    { Modulation::NAME_32QAM,       { "32QAM", "QAM32" } },
    { Modulation::NAME_64QAM,       { "64QAM", "QAM64" } },
    { Modulation::NAME_128QAM,      { "128QAM", "QAM128" } },
    { Modulation::NAME_256QAM,      { "256QAM", "QAM256" } }
};

//!************************************************************************
//! Constructor
//!************************************************************************
Modulation::Modulation()
{
    std::string duplicate;

    if( !verifyAliasNames( duplicate ) )
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle( "Modulation names - duplicate" );
        msgBox.setText( "The name \"" + QString::fromStdString( duplicate ) + "\" appears in more than one place." );
        msgBox.exec();
    }
}


//!************************************************************************
//! Singleton
//!
//! @returns the instance of the object
//!************************************************************************
Modulation* Modulation::getInstance()
{
    if( !sInstance )
    {
        sInstance = new Modulation;
    }

    return sInstance;
}


//!************************************************************************
//! Get the family a modulation belongs to
//!
//! @returns The modulation family
//!************************************************************************
Modulation::ModulationFamily Modulation::getFamily
    (
    const ModulationName aName      //!< modulation name
    ) const
{
    ModulationFamily modFam = FAMILY_UNKNOWN;

    switch( aName )
    {
        case NAME_AM_SSB:
        case NAME_AM_SSB_WC:
        case NAME_AM_SSB_SC:
        case NAME_AM_DSB:
        case NAME_AM_DSB_WC:
        case NAME_AM_DSB_SC:
        case NAME_AM_USB:
        case NAME_AM_LSB:
            modFam = FAMILY_AM;
            break;

        case NAME_FM:
        case NAME_WBFM:
            modFam = FAMILY_FM;
            break;

        case NAME_PM:
            modFam = FAMILY_PM;
            break;

        case NAME_16APSK:
        case NAME_32APSK:
        case NAME_64APSK:
        case NAME_128APSK:
            modFam = FAMILY_APSK;
            break;

        case NAME_OOK:
        case NAME_4ASK:
        case NAME_8ASK:
            modFam = FAMILY_ASK;
            break;

        case NAME_2FSK:
        case NAME_4FSK:
        case NAME_8FSK:
        case NAME_16FSK:
        case NAME_GFSK:
        case NAME_CPFSK:
        case NAME_GMSK:
            modFam = FAMILY_FSK;
            break;

        case NAME_BPSK:
        case NAME_QPSK:
        case NAME_8PSK:
        case NAME_16PSK:
        case NAME_32PSK:
        case NAME_64PSK:
        case NAME_OQPSK:
            modFam = FAMILY_PSK;
            break;

        case NAME_4PAM:
        case NAME_8PAM:
        case NAME_16PAM:
            modFam = FAMILY_PAM;
            break;

        case NAME_4QAM:
        case NAME_8QAM:
        case NAME_16QAM:
        case NAME_32QAM:
        case NAME_64QAM:
        case NAME_128QAM:
        case NAME_256QAM:
            modFam = FAMILY_QAM;
            break;

        default:
            break;
    }

    return modFam;
}


//!************************************************************************
//! Get the string for the family a modulation belongs to
//!
//! @returns The string containing the modulation family
//!************************************************************************
std::string Modulation::getFamilyString
    (
    const ModulationName aName      //!< modulation name
    ) const
{
    std::string modFamStr = "Unknown";

    switch( getFamily( aName ) )
    {
        case FAMILY_AM:
            modFamStr = "Amplitude Modulation";
            break;
        case FAMILY_FM:
            modFamStr = "Frequency Modulation";
            break;
        case FAMILY_PM:
            modFamStr = "Phase Modulation";
            break;
        case FAMILY_APSK:
            modFamStr = "Amplitude and Phase-Shift Keying";
            break;
        case FAMILY_ASK:
            modFamStr = "Amplitude-Shift Keying";
            break;
        case FAMILY_FSK:
            modFamStr = "Frequency-Shift Keying";
            break;
        case FAMILY_PSK:
            modFamStr = "Phase-Shift Keying";
            break;
        case FAMILY_PAM:
            modFamStr = "Pulse-Amplitude Modulation";
            break;
        case FAMILY_QAM:
            modFamStr = "Quadrature Amplitude Modulation";
            break;
        default:
            break;
    }

    return modFamStr;
}


//!************************************************************************
//! Get a modulation name based on a given string
//!
//! @returns The modulation name
//!************************************************************************
Modulation::ModulationName Modulation::getModulationName
    (
    const std::string aString       //!< string
    ) const
{
    ModulationName modName = NAME_UNKNOWN;
    bool found = false;

    for( auto mapIt = MODULATION_NAME_ALIAS.begin(); mapIt != MODULATION_NAME_ALIAS.end(); mapIt++ )
    {
        if( NAME_UNKNOWN == mapIt->first )
        {
            continue;
        }

        std::vector<std::string> strVec = mapIt->second;

        for( size_t strIt = 0; strIt < strVec.size(); strIt++ )
        {
           if( 0 == strVec.at( strIt ).compare( aString ) )
           {
               modName = mapIt->first;
               found = true;
               break;
           }
        }

        if( found )
        {
            break;
        }
    }

    return modName;
}


//!************************************************************************
//! Get a string corresponding to the modulation name.
//! If the modulation is defined by multiple names, it returns the first
//! string from the vector.
//!
//! @returns The string corresponding to the modulation name
//!************************************************************************
std::string Modulation::getModulationString
    (
    const ModulationName aName      //!< modulation name
    ) const
{
    return MODULATION_NAME_ALIAS.at( aName ).at( 0 );
}


//!************************************************************************
//! Get the type a modulation belongs to
//!
//! @returns The modulation type
//!************************************************************************
Modulation::ModulationType Modulation::getType
    (
    const ModulationName aName      //!< modulation name
    ) const
{
    ModulationType modType = TYPE_UNKNOWN;

    if( aName >= NAME_AM_SSB && aName <= NAME_PM )
    {
        modType = TYPE_ANALOG;
    }
    else if( aName >= NAME_16APSK && aName <= NAME_256QAM )
    {
        modType = TYPE_DIGITAL;
    }

    return modType;
}


//!************************************************************************
//! Get the string for the type a modulation belongs to
//!
//! @returns The string containing the modulation type
//!************************************************************************
std::string Modulation::getTypeString
    (
    const ModulationName aName      //!< modulation name
    ) const
{
    std::string modTypeStr = "Unknown";

    switch( getType( aName ) )
    {
        case TYPE_ANALOG:
            modTypeStr = "Analog";
            break;

        case TYPE_DIGITAL:
            modTypeStr = "Digital";
            break;

        default:
            break;
    }

    return modTypeStr;
}


//!************************************************************************
//! Verify if the alias names are unique
//!
//! @returns true if there are no duplicates in the alias names
//!************************************************************************
bool Modulation::verifyAliasNames
    (
    std::string& aDuplicate     //!< first duplicate string found
    )
{
    bool status = true;

    for( auto mapItOne = MODULATION_NAME_ALIAS.begin(); mapItOne != MODULATION_NAME_ALIAS.end(); mapItOne++ )
    {
        if( NAME_UNKNOWN == mapItOne->first )
        {
            continue;
        }

        std::vector<std::string> strOneVec = mapItOne->second;
        size_t lenOne = strOneVec.size();

        if( !lenOne )
        {
            status = false;
            break;
        }

        for( auto mapItTwo = std::next( mapItOne ); mapItTwo != MODULATION_NAME_ALIAS.end(); mapItTwo++ )
        {
            std::vector<std::string> strTwoVec = mapItTwo->second;
            size_t lenTwo = strTwoVec.size();

            if( !lenTwo )
            {
                status = false;
                break;
            }

            for( size_t strTwoIt = 0; strTwoIt < lenTwo; strTwoIt++ )
            {
                for( size_t strOneIt = 0; strOneIt < lenOne; strOneIt++ )
                {
                    if( 0 == strTwoVec.at( strTwoIt ).compare( strOneVec.at( strOneIt ) ) )
                    {
                        aDuplicate = strOneVec.at( strOneIt );
                        status = false;
                        break;
                    }
                }
            }
        }
    }

    return status;
}
