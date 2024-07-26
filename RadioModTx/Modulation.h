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
Modulation.h

This file contains the definitions for modulation.
*/

#ifndef Modulation_h
#define Modulation_h

#include <cstdint>
#include <map>
#include <string>
#include <vector>


//************************************************************************
// Class for handling the modulation
//************************************************************************
class Modulation
{
    //************************************************************************
    // constants and types
    //************************************************************************
    public:
        typedef enum : uint8_t
        {
            TYPE_UNKNOWN,
            TYPE_ANALOG,
            TYPE_DIGITAL
        }ModulationType;

        typedef enum : uint8_t
        {
            FAMILY_UNKNOWN,
            //*********
            // analog
            //*********
            FAMILY_AM,      //!< Amplitude Modulation
            FAMILY_FM,      //!< Frequency Modulation
            FAMILY_PM,      //!< Phase Modulation
            //*********
            // digital
            //*********
            // SK
            FAMILY_APSK,    //!< Amplitude and Phase-Shift Keying
            FAMILY_ASK,     //!< Amplitude-Shift Keying
            FAMILY_FSK,     //!< Frequency-Shift Keying
            FAMILY_PSK,     //!< Phase-Shift Keying
            // AM
            FAMILY_PAM,     //!< Pulse-Amplitude Modulation
            FAMILY_QAM      //!< Quadrature Amplitude Modulation
        }ModulationFamily;

        typedef enum : uint16_t
        {
            NAME_UNKNOWN,

            //*********
            // analog
            //*********
            // AM
            NAME_AM_SSB,        //!< Amplitude Modulation - Single Sideband
            NAME_AM_SSB_WC,     //!< Amplitude Modulation - Single Sideband - With Carrier
            NAME_AM_SSB_SC,     //!< Amplitude Modulation - Single Sideband - Suppressed Carrier
            NAME_AM_DSB,        //!< Amplitude Modulation - Double Sideband
            NAME_AM_DSB_WC,     //!< Amplitude Modulation - Double Sideband - With Carrier
            NAME_AM_DSB_SC,     //!< Amplitude Modulation - Double Sideband - Suppressed Carrier
            NAME_AM_USB,        //!< Amplitude Modulation - Upper Sideband
            NAME_AM_LSB,        //!< Amplitude Modulation - Lower Sideband
            // FM
            NAME_FM,            //!< Frequency Modulation
            NAME_WBFM,          //!< Frequency Modulation - Wideband
            // PM
            NAME_PM,            //!< Phase Modulation

            //*********
            // digital
            //*********
            // APSK - Amplitude and Phase-Shift Keying
            NAME_16APSK,
            NAME_32APSK,
            NAME_64APSK,
            NAME_128APSK,
            // ASK - Amplitude-Shift Keying
            NAME_OOK,           //!< On-Off Keying (aka 2ASK)
            NAME_4ASK,
            NAME_8ASK,
            // FSK - Frequency-Shift Keying
            NAME_2FSK,
            NAME_4FSK,
            NAME_8FSK,
            NAME_16FSK,
            NAME_GFSK,          //!< Gaussian FSK
            NAME_CPFSK,         //!< Continuous Phase FSK
            NAME_GMSK,          //!< Gaussian Minimum-Shift Keying
            // PSK - Phase-Shift Keying
            NAME_BPSK,          //!< Binary PSK (aka 2PSK)
            NAME_QPSK,          //!< Quadrature PSK (aka 4PSK)
            NAME_8PSK,
            NAME_16PSK,
            NAME_32PSK,
            NAME_64PSK,
            NAME_OQPSK,         //!< Offset Quadrature PSK
            // PAM - Pulse-Amplitude Modulation
            NAME_4PAM,
            NAME_8PAM,
            NAME_16PAM,
            // QAM - Quadrature Amplitude Modulation
            NAME_4QAM,
            NAME_8QAM,
            NAME_16QAM,
            NAME_32QAM,
            NAME_64QAM,
            NAME_128QAM,
            NAME_256QAM
        }ModulationName;

        static const std::map<ModulationName, std::vector<std::string>> MODULATION_NAME_ALIAS;

    //************************************************************************
    // functions
    //************************************************************************
    public:
        Modulation();

        static Modulation* getInstance();

        ModulationFamily getFamily
            (
            const ModulationName    aName       //!< modulation name
            ) const;

        std::string getFamilyString
            (
            const ModulationName    aName       //!< modulation name
            ) const;

        ModulationName getModulationName
            (
            const std::string       aString     //!< string
            ) const;

        std::string getModulationString
            (
            const ModulationName    aName       //!< modulation name
            ) const;

        ModulationType getType
            (
            const ModulationName    aName       //!< modulation name
            ) const;

        std::string getTypeString
            (
            const ModulationName    aName       //!< modulation name
            ) const;

    private:
        bool verifyAliasNames
            (
            std::string&            aDuplicate  //!< first duplicate string found
            );


    //************************************************************************
    // variables
    //************************************************************************
    private:
        static Modulation*      sInstance;  //!< singleton
};

#endif // Modulation_h
