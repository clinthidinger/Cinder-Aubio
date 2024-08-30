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

#include "CinderAubioMelBands.h"
#include <cinder/app/App.h>


CinderAubioMelBands::CinderAubioMelBands()
    : CinderAubioMelBands( kDefaultMethod, kDefaultBufSize, kDefaultHopSize, kDefaultSampleRate )
{

}

CinderAubioMelBands::CinderAubioMelBands( const std::string &method, int bufSize, int hopSize, int sampleRate )
    : CinderAubioBlock( method, bufSize, hopSize, sampleRate )
{
    constexpr int kDefaultNBands = 40;
    mNBands = kDefaultNBands;
    mPv = AubioPvocPtr( new_aubio_pvoc( bufSize, hopSize ), [] ( aubio_pvoc_t *pv ) {  del_aubio_pvoc( pv ); } );
    mSpectrum = AubioCvecPtr( new_cvec( bufSize ), [] ( cvec_t *s ) { del_cvec( s ); } );
    mFb = AubioFilterBankPtr( new_aubio_filterbank( mNBands, bufSize ), [] ( aubio_filterbank_t *fb ) { del_aubio_filterbank( fb ); } );
    aubio_filterbank_set_mel_coeffs_slaney( mFb.get(), sampleRate );
    mBands = AubioFvecPtr( new_fvec( mNBands ), [] ( fvec_t *f ) { del_fvec( f ); } );
    mEnergies = mBands->data;

    if( mPv && mFb ) {
        ci::app::console() << "created CinderAubioMelBands(" << method
            << ", " << mBufSize
            << ", " << mHopSize
            << ", " << sampleRate
            << ")";
    }
}

CinderAubioMelBands::CinderAubioMelBands( CinderAubioMelBands &&other ) noexcept
    : CinderAubioBlock( std::move( other ) )
{
    mPv = std::move( other.mPv );
    mSpectrum = std::move( other.mSpectrum );
    mFb = std::move( other.mFb );
    mBands = std::move( other.mBands );
}

CinderAubioMelBands &CinderAubioMelBands::operator=( CinderAubioMelBands &&rhs ) noexcept
{
    if( this != &rhs )
    {
        CinderAubioBlock::operator=( std::move( rhs ) );
        mPv = std::move( rhs.mPv );
        mSpectrum = std::move( rhs.mSpectrum );
        mFb = std::move( rhs.mFb );
        mBands = std::move( rhs.mBands );
    }
    return *this;
}

void CinderAubioMelBands::blockAudioIn()
{
    aubio_pvoc_do( mPv.get(), mAubioInput.get(), mSpectrum.get() );
    aubio_filterbank_do( mFb.get(), mSpectrum.get(), mBands.get());
}
