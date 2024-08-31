/*
 * This sample illustrates how to get audio data from an input device, such as a microphone,
 * with audio::InputDeviceNode. It then visualizes the input in the frequency domain. The frequency
 * spectrum analysis is accomplished with an audio::MonitorSpectralNode.
 *
 * The plot is similar to a typical spectrogram, where the x-axis represents the linear
 * frequency bins (0 - samplerate / 2) and the y-axis is the magnitude of the frequency
 * bin in normalized decibels (0 - 100).
 *
 * author: Richard Eakin (2014)
 */

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/TextureFont.h"

#include "cinder/audio/audio.h"
#include "AudioDrawUtils.h"
#include "CinderAubioBeat.h"
#include "CinderAubioBlock.h"
#include "CinderAubioMelBands.h"
#include "CinderAubioOnset.h"
#include "CinderAubioPitch.h"


using namespace ci;
using namespace ci::app;
using namespace std;

class AubioInputAnalyzerApp : public App {
public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	void drawSpectralCentroid();
	void drawLabels();
	void printBinInfo( int mouseX );

	audio::InputDeviceNodeRef		     mInputDeviceNode;
	audio::MonitorSpectralNodeRef	     mMonitorSpectralNode;
	std::vector<float>				     mMagSpectrum;

	SpectrumPlot					     mSpectrumPlot;
	gl::TextureFontRef				     mTextureFont;
	std::unique_ptr<CinderAubioBeat>     mAubioBeat;
	std::unique_ptr<CinderAubioPitch>    mAubioPitch;
	std::unique_ptr<CinderAubioOnset>    mAubioOnset;
	std::unique_ptr<CinderAubioMelBands> mAubioBands;
	bool                                 mHasBeat{ false };
	bool                                 mHasOnset{ false };
};

void AubioInputAnalyzerApp::setup()
{
	auto ctx = audio::Context::master();

	// The InputDeviceNode is platform-specific, so you create it using a special method on the Context:
	mInputDeviceNode = ctx->createInputDeviceNode();

	mAubioBeat = std::make_unique<CinderAubioBeat>( "default", 2048, 1024, 44100 );
	mAubioPitch = std::make_unique<CinderAubioPitch>( "default", 2048, 1024, 44100 );
	mAubioOnset = std::make_unique<CinderAubioOnset>( "default", 2048, 1024, 44100 );
	mAubioBands = std::make_unique<CinderAubioMelBands>( "default", 2048, 1024, 44100 );

	mAubioBeat->getGotBeatSignal().connect( [this] ( float beat ) { mHasBeat = true; } );
	mAubioOnset->getGotOnsetSignal().connect( [this] ( float onset ) { mHasOnset = true; } );

	// By providing an FFT size double that of the window size, we 'zero-pad' the analysis data, which gives
	// an increase in resolution of the resulting spectrum data.
	auto monitorFormat = audio::MonitorSpectralNode::Format().fftSize( 2048 ).windowSize( 1024 );
	//auto monitorFormat = audio::MonitorSpectralNode::Format().fftSize( 512 ).windowSize( 256 );
	mMonitorSpectralNode = ctx->makeNode( new audio::MonitorSpectralNode( monitorFormat ) );

	mInputDeviceNode >> mMonitorSpectralNode;

	mInputDeviceNode->enable();
	ctx->enable();

	getWindow()->setTitle( mInputDeviceNode->getDevice()->getName() );
}

void AubioInputAnalyzerApp::mouseDown( MouseEvent event )
{
	audio::Buffer buffer = mMonitorSpectralNode->getBuffer();
	console() << "number of channels: " << buffer.getNumChannels() << std::endl;
	console() << "number of frames: " << buffer.getNumFrames() << std::endl;
	console() << "size: " << buffer.getSize() << std::endl;

	if( mSpectrumPlot.getBounds().contains( event.getPos() ) ) {
		printBinInfo( event.getX() );
	}
}

void AubioInputAnalyzerApp::update()
{
	mSpectrumPlot.setBounds( Rectf( 40, 40, ( float ) getWindowWidth() - 40, ( float ) getWindowHeight() - 40 ) );

	audio::Buffer buffer = mMonitorSpectralNode->getBuffer();
	
	float *data = buffer.getData();
	const int numChannels = buffer.getNumChannels();
	const int bufferSize = buffer.getNumFrames();
	mAubioOnset->audioIn( data, bufferSize, numChannels );
	mAubioPitch->audioIn( data, bufferSize, numChannels );
	mAubioBeat->audioIn( data, bufferSize, numChannels );
	mAubioBands->audioIn( data, bufferSize, numChannels );

	// We copy the magnitude spectrum out from the Node on the main thread, once per update:
	mMagSpectrum = mMonitorSpectralNode->getMagSpectrum();
}

void AubioInputAnalyzerApp::draw()
{
	gl::clear();
	gl::enableAlphaBlending();

	//mSpectrumPlot.draw( mMonitorSpectralNode->getMagSpectrum() );
	const std::vector<float> spectrum( mAubioBands->getEnergies(), mAubioBands->getEnergies() + mAubioBands->getNBands() );
	mSpectrumPlot.draw( spectrum );
	drawSpectralCentroid();
	drawLabels();

	if( mHasBeat ) {
		const gl::ScopedColor scopedClr( ColorAf( 1, 1, 1 ) );
		gl::pushModelView();
		gl::drawSolidRect( { 0.0f, 0.0f, ( float ) getWindowWidth(), ( float ) getWindowHeight() } );
		gl::popModelView();
		mHasBeat = false;
	}
	if( mHasOnset ) {
		const gl::ScopedColor scopedClr( ColorAf( 0, 0, 1 ) );
		gl::pushModelView();
		gl::drawSolidCircle( { getWindowWidth() * 0.5f, getWindowHeight() * 0.5f }, getWindowWidth() * 0.33f );
		gl::popModelView();
		mHasOnset = false;
	}

	// getLatestPitch()
	gl::color( 0, 0.9f, 0.9f );
}

void AubioInputAnalyzerApp::drawSpectralCentroid()
{
	// The spectral centroid is largely correlated with 'brightness' of a sound. It is the center of mass of all frequency values.
	// See the note on audio::MonitorSpectralNode::getSpectralCentroid() - it may be analyzing a more recent magnitude spectrum
	// than what we're drawing in the SpectrumPlot. It is not a problem for this simple sample, but if you need a more precise
	// value, use audio::dsp::spectralCentroid() directly.

	float spectralCentroid = mMonitorSpectralNode->getSpectralCentroid();

	float nyquist = ( float ) audio::master()->getSampleRate() / 2.0f;
	Rectf bounds = mSpectrumPlot.getBounds();

	float freqNormalized = spectralCentroid / nyquist;
	float barCenter = bounds.x1 + freqNormalized * bounds.getWidth();
	Rectf verticalBar = { barCenter - 2, bounds.y1, barCenter + 2, bounds.y2 };

	gl::ScopedColor colorScope( 0.85f, 0.45f, 0, 0.4f ); // transparent orange
	gl::drawSolidRect( verticalBar );
}

void AubioInputAnalyzerApp::drawLabels()
{
	if( !mTextureFont ) {
		mTextureFont = gl::TextureFont::create( Font( Font::getDefault().getName(), 16 ) );
	}
	// draw x-axis label
	string freqLabel = "Frequency (hertz)";
	mTextureFont->drawString( freqLabel, vec2( getWindowCenter().x - mTextureFont->measureString( freqLabel ).x / 2, ( float ) getWindowHeight() - 20 ) );

	// draw y-axis label
	string dbLabel = "Magnitude (decibels, linear)";
	gl::pushModelView();
	gl::translate( 30, getWindowCenter().y + mTextureFont->measureString( dbLabel ).x / 2 );
	gl::rotate( -M_PI / 2 );
	mTextureFont->drawString( dbLabel, vec2( 0 ) );
	gl::popModelView();
}

void AubioInputAnalyzerApp::printBinInfo( int mouseX )
{
	size_t numBins = mMonitorSpectralNode->getFftSize() / 2;
	size_t bin = std::min<size_t>( numBins - 1, size_t( ( numBins * ( mouseX - mSpectrumPlot.getBounds().x1 ) ) / mSpectrumPlot.getBounds().getWidth() ) );

	float binFreqWidth = mMonitorSpectralNode->getFreqForBin( 1 ) - mMonitorSpectralNode->getFreqForBin( 0 );
	float freq = mMonitorSpectralNode->getFreqForBin( bin );
	float mag = audio::linearToDecibel( mMagSpectrum[bin] );

	console() << "bin: " << bin << ", freqency (hertz): " << freq << " - " << freq + binFreqWidth << ", magnitude (decibels): " << mag << endl;
}

CINDER_APP( AubioInputAnalyzerApp, RendererGl( RendererGl::Options().msaa( 8 ) ) )