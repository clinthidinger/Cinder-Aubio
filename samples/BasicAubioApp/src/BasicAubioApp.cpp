#if 0
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/SamplePlayerNode.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/audio/FilterNode.h"
#include "cinder/audio/Utilities.h"
#include <cinder/CinderImGui.h>

using namespace ci;
using namespace ci::app;

class BasicAubioApp : public App
{
  public:
    BasicAubioApp();
    virtual ~BasicAubioApp() = default;
    BasicAubioApp( const BasicAubioApp &other ) = delete;
    BasicAubioApp &operator =( const BasicAubioApp &rhs ) = delete;
    BasicAubioApp( const BasicAubioApp &&other ) = delete;
    BasicAubioApp &operator =( const BasicAubioApp &&rhs ) = delete;
    
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
    void setup() override;
    void fileDrop( FileDropEvent event ) override;
    void keyDown( KeyEvent event ) override;
    void setupAudioNodes();
    
    audio::InputDeviceNodeRef mInputNode;
    audio::MonitorNodeRef mMonitorNode;
    //audio::BufferDynamicSpectralRef mSpectralNode;
    audio::MonitorSpectralNodeRef mSpectralNode;

    audio::FilePlayerNodeRef        mFilePlayerNode;
    //audio::MonitorNodeRef           mMonitorNode1;
    //audio::MonitorNodeRef           mMonitorNode2;
    audio::GainNodeRef              mGainNode;
    //audio::FilterLowPassNodeRef     mLowpassNode;
    //audio::FilterHighPassNodeRef    mHighpassNode;

    bool mUseInput{ true };
    float mGain{ 1.0f };
    float mBeatThreshold{ 0.5f };
    bool mIsBeat{ false };
};

//------------------------------------------------------------------------------
BasicAubioApp::BasicAubioApp()
{
    
}

void BasicAubioApp::setup()
{
    setupAudioNodes();
    ImGui::Initialize();
}

//------------------------------------------------------------------------------
void BasicAubioApp::setupAudioNodes()
{
    mFilePlayerNode = audio::Context::master()->makeNode( new audio::FilePlayerNode() );
    mGainNode = audio::Context::master()->makeNode( new audio::GainNode( 1.0f ) );

    mInputNode = audio::Context::master()->createInputDeviceNode();
    //mSpectralNode = audio::BufferDynamicSpectral::create( mMonitorNode->getFormat() );
    mMonitorNode = audio::Context::master()->makeNode( new audio::MonitorNode() );
    //mSpectralNode = audio::Context::master()->makeNode( new audio::BufferDynamicSpectral( ) );// mMonitorNode->getFormat() ) );

    //mFilePlayerNode >> mGainNode >> audio::Context::master()->getOutput();
    //mGainNode >> audio::Context::master()->getOutput();
    
    
    //mMonitorNode2 = audio::Context::master()->makeNode( new audio::MonitorNode() );
    //mLowpassNode = audio::Context::master()->makeNode( new audio::FilterLowPassNode() );
    //mHighpassNode = audio::Context::master()->makeNode( new audio::FilterHighPassNode() );
    
    //mLowpassNode >> mMonitorNode1;
    //mHighpassNode >> mMonitorNode2;

    auto format = audio::MonitorSpectralNode::Format().fftSize( 512 ).windowSize( 256 );
    mSpectralNode = audio::Context::master()->makeNode( new audio::MonitorSpectralNode( format ) );


    mInputNode >> mGainNode >> mMonitorNode >> mSpectralNode >> audio::Context::master()->getOutput();
    audio::Context::master()->enable();
    mGainNode->enable();
    mMonitorNode->enable();
    mSpectralNode->enable();
    mInputNode->enable();
}

//------------------------------------------------------------------------------
void BasicAubioApp::fileDrop( FileDropEvent event )
{
    mInputNode->disable();
    mUseInput = false;
    fs::path filePath = event.getFile( 0 );
    getWindow()->setTitle( filePath.filename().string() );
    
    //audio::Context::master()->disable();
    audio::SourceFileRef sourceFile = audio::load( loadFile( filePath ) );
    
    mFilePlayerNode->setSourceFile( sourceFile );
    mFilePlayerNode->setLoopEnabled();
    //mFilePlayerNode >> mGainNode;
    
    //mFilePlayerNode >> mLowpassNode;
    //mFilePlayerNode >> mHighpassNode;
    
    //audio::Context::master()->enable();
    //mFilePlayerNode->start();

    mFilePlayerNode >> mGainNode >> mMonitorNode >> mSpectralNode >> audio::Context::master()->getOutput();
    
    audio::Context::master()->enable();
    mFilePlayerNode->start();
    //mLowpassNode->enable();
    //mLowpassNode->setMode();
}

//------------------------------------------------------------------------------
void BasicAubioApp::keyDown( KeyEvent event )
{
    if( event.getCode() == KeyEvent::KEY_SPACE )
    {
        if( mFilePlayerNode->isEnabled() )
        {
            mFilePlayerNode->stop();
        }
        else
        {
            mFilePlayerNode->start();
        }
    }
    else if( event.getCode() == KeyEvent::KEY_UP )
    {
        if( mGainNode->isEnabled() )
        {
            mGainNode->setValue( mGainNode->getValue() + 0.1f );
        }
    }
    else if( event.getCode() == KeyEvent::KEY_DOWN )
    {
        if( mGainNode->isEnabled() )
        {
            mGainNode->setValue( mGainNode->getValue() - 0.1f );
        }
    }
}

//------------------------------------------------------------------------------
void BasicAubioApp::mouseDown( MouseEvent event )
{
}

//------------------------------------------------------------------------------
void BasicAubioApp::update()
{

    //audio::Buffer dynamicBuffer = mMonitorNode->getBuffer();
    /*
    auto &buffer = mSpectralNode->getBuffer();
    // Analyze low-frequency spectrum (adjust frequency range as needed)
    float lowFreqEnergy = 0.0f;
    float sampleRate = mSpectralNode->getSampleRate();
    auto const numFrames = buffer.getNumFrames();
    int startBin = 0;// static_cast< int >( 20.0f * numFrames / sampleRate );  // Adjust frequency range
    int endBin = 64;// static_cast< int >( 150.0f * numFrames / sampleRate );   // accordingly

    for( int i = startBin; i <= endBin; ++i ) 
    {
        lowFreqEnergy += buffer.getChannel( 0 )[i] * buffer.getChannel( 0 )[i];
    }

    lowFreqEnergy /= ( endBin - startBin + 1 );

    // Beat detection logic
    if( lowFreqEnergy * mGain > mBeatThreshold ) 
    {
        mIsBeat = true;
        // Adjust beat visualization position as needed
        //mBeatCirclePos = vec2( getWindowWidth() * 0.25f, getWindowHeight() * 0.5f );
    }
    else {
        mIsBeat = false;
        // Adjust non-beat visualization position as needed
        //mAndBeatCirclePos = vec2( getWindowWidth() * 0.75f, getWindowHeight() * 0.5f );
    }

    */
    /////

    /*
    // Update audio input
    audio::Buffer dynamicBuffer = mMonitorNode->getBuffer();

    // Analyze low-frequency spectrum (adjust frequency range as needed)
    float lowFreqEnergy = 0.0f;
    int startBin = audio::toSampleRate( 20.0f, dynamicBuffer.getSampleRate() ) * 0.5;  // Adjust frequency range
    int endBin = audio::toSampleRate( 150.0f, dynamicBuffer.getSampleRate() ) * 0.5;   // accordingly

    for( int i = startBin; i <= endBin; ++i ) 
    {
        lowFreqEnergy += dynamicBuffer.getChannel( 0 )[i] * dynamicBuffer.getChannel( 0 )[i];
    }

    lowFreqEnergy /= ( endBin - startBin + 1 );

    // Beat detection logic
    if( lowFreqEnergy * mGain > mBeatThreshold ) 
    {
        mIsBeat = true;
        // Adjust beat visualization position as needed
        //mBeatCirclePos = vec2( getWindowWidth() * 0.25f, getWindowHeight() * 0.5f );
    }
    else 
    {
        mIsBeat = false;
        // Adjust non-beat visualization position as needed
        //mAndBeatCirclePos = vec2( getWindowWidth() * 0.75f, getWindowHeight() * 0.5f );
    }
    */
    /////

    //*
    // Update audio input
    //audio::Buffer dynamicBuffer = mMonitorNode->getBuffer();
    auto dynamicSpectrum = mSpectralNode->getBuffer();

    // Analyze low-frequency spectrum (adjust frequency range as needed)
    float lowFreqEnergy = 0.0f;

    for( size_t i = 0; i < dynamicSpectrum.getNumFrames(); ++i ) 
    {
        lowFreqEnergy += dynamicSpectrum[i];//dynamicSpectrum.getMagSpectrum()[i];
    }

    lowFreqEnergy /= dynamicSpectrum.getNumFrames();

    // Beat detection logic
    if( lowFreqEnergy * mGain > mBeatThreshold ) 
    {
        mIsBeat = true;
        // Adjust beat visualization position as needed
        //mBeatCirclePos = vec2( getWindowWidth() * 0.25f, getWindowHeight() * 0.5f );
    }
    else 
    {
        mIsBeat = false;
        // Adjust non-beat visualization position as needed
        //mAndBeatCirclePos = vec2( getWindowWidth() * 0.75f, getWindowHeight() * 0.5f );
    }
    //*/
}

void BasicAubioApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    float radius = 100;
    ivec2 center = getWindowCenter();
    /*if( mMonitorNode1->isEnabled() )
    {
        gl::color( ColorA( 1.0, 0.0, 0.0, 0.5 ) );
        ci::gl::drawSolidCircle( center + ivec2( 0, 0 ), radius * mMonitorNode1->getVolume() );
    }*/
    
    /*if( mMonitorNode2->isEnabled() )
    {
        gl::color( ColorA( 0.0, 0.0, 1.0, 0.5 ) );
        ci::gl::drawSolidCircle( center + ivec2( 0, 0 ), radius * mMonitorNode2->getVolume() );
    }*/

    gl::clear( Color::black() );

    // Draw beat circle
    if( mIsBeat )
    {
        gl::color( Color( 0, 0, 1 ) );  // Blue
        gl::drawSolidCircle( center, 50.0f );
    }

    // Draw non-beat circle
    else 
    {
        gl::color( Color( 1, 1, 1 ) );  // White
        gl::drawSolidCircle( center, 50.0f );
    }
}

CINDER_APP( BasicAubioApp, RendererGl(),
    [&]( App::Settings *settings )
    {
        settings->setWindowSize( 1280, 720 );
    })
#endif