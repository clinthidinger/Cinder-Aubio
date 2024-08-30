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

#include "CinderAubioBlock.h"

CinderAubioBlock::CinderAubioBlock( const std::string &method, int bufSize, int hopSize, int samplerate )
{
    mHopSize = static_cast<uint_t>( hopSize );
    mBufSize = static_cast<uint_t>( bufSize );
    mAubioInput = AubioFvecPtr( new_fvec( mHopSize ), [] ( fvec_t *v ) { del_fvec( v ); } );
    mAubioOutput = AubioFvecPtr( new_fvec( 1 ), [] ( fvec_t *v ) { del_fvec( v ); } );
    mCurPos = 0;
}

CinderAubioBlock::CinderAubioBlock( CinderAubioBlock &&other ) noexcept
{
    mAubioInput = std::move( other.mAubioInput );
    mAubioOutput = std::move( other.mAubioOutput );
}

CinderAubioBlock &CinderAubioBlock::operator=( CinderAubioBlock &&rhs ) noexcept
{
    if( this != &rhs )
    {
        mAubioInput = std::move( rhs.mAubioInput );
        mAubioOutput = std::move( rhs.mAubioOutput );
    }
    return *this;
}

void CinderAubioBlock::audioIn( float *input, int bufferSize, int nChannels )
{
    for( int i = 0; i < bufferSize; ++i ) {
        // Downmix into aubio_input.
        mAubioInput->data[mCurPos] = 0.0f;
        for( int j = 0; j < nChannels; ++j ) {
            mAubioInput->data[mCurPos] += input[i * nChannels + j];
        }
        mAubioInput->data[mCurPos] /= static_cast<smpl_t>( nChannels );
        // Run aubio block when appropriate.
        ++mCurPos;// += 1;
        if( mCurPos == mHopSize )
        {
            blockAudioIn();
            mCurPos = 0;
        }
    }
}

void CinderAubioBlock::blockAudioIn()
{
}
