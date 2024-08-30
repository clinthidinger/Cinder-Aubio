/*
  Copyright (C) 2015 Paul Brossier <piem@aubio.org>

  This file is ported from ofxAubio.

  ofxAubio is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  aubio is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with aubio.  If not, see <http://www.gnu.org/licenses/>.

*/


#pragma once

#include <string>
#include <aubio.h>
#include "CinderAubioBlock.h"

class CinderAubioMelBands : public CinderAubioBlock {

public:
    CinderAubioMelBands();
    CinderAubioMelBands( const std::string &method, int bufSize, int hopSize, int sampleRate );
    ~CinderAubioMelBands() override = default;
    CinderAubioMelBands( const CinderAubioMelBands &other ) = delete;
    CinderAubioMelBands &operator=( const CinderAubioMelBands &rhs ) = delete;
    CinderAubioMelBands( CinderAubioMelBands &&other ) noexcept;
    CinderAubioMelBands &operator=( CinderAubioMelBands &&other ) noexcept;

    int getNBands() const;
    const float *getEnergies() const;

private:
    void blockAudioIn() override;
    
    using AubioPvocPtr = std::unique_ptr<aubio_pvoc_t, std::function<void( aubio_pvoc_t * )>>;
    using AubioCvecPtr = std::unique_ptr<cvec_t, std::function<void( cvec_t * )>>;
    using AubioFvecPtr = std::unique_ptr<fvec_t, std::function<void( fvec_t * )>>;
    using AubioFilterBankPtr = std::unique_ptr<aubio_filterbank_t, std::function<void( aubio_filterbank_t * )>>;

    AubioPvocPtr mPv;
    AubioCvecPtr mSpectrum;
    AubioFilterBankPtr mFb;
    AubioFvecPtr mBands;
    int mNBands{ 0 };
    float *mEnergies{ nullptr };
};

inline int CinderAubioMelBands::getNBands() const { return mNBands; }
inline const float *CinderAubioMelBands::getEnergies() const { return mEnergies; }
