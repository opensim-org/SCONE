#include "QPlayControl.h"

#include <QToolButton>
#include <QWidget>
#include <QComboBox>

QPlayControl::QPlayControl( QWidget *parent /*= 0 */ )
{
	playButton = new QToolButton( this );
	playButton->setIcon( style()->standardIcon( QStyle::SP_MediaPlay ) );
	connect( playButton, SIGNAL( clicked() ), this, SIGNAL( play() ) );

	stopButton = new QToolButton( this );
	stopButton->setIcon( style()->standardIcon( QStyle::SP_MediaStop ) );
	connect( stopButton, SIGNAL( clicked() ), this, SIGNAL( stop() ) );

	nextButton = new QToolButton( this );
	nextButton->setIcon( style()->standardIcon( QStyle::SP_MediaSkipForward ) );
	connect( nextButton, SIGNAL( clicked() ), this, SIGNAL( next() ) );

	previousButton = new QToolButton( this );
	previousButton->setIcon( style()->standardIcon( QStyle::SP_MediaSkipBackward ) );
	connect( previousButton, SIGNAL( clicked() ), this, SIGNAL( previous() ) );

	loopButton = new QToolButton( this );
	loopButton->setCheckable( true );
	loopButton->setIcon( style()->standardIcon( QStyle::SP_BrowserReload ) );

	label = new QLabel( this );
	label->setText( "0.000" );
	slider = new QSlider( Qt::Horizontal, this );
	connect( slider, SIGNAL( valueChanged( int ) ), this, SLOT( updateSlider( int ) ) );

	slowMotionBox = new QComboBox( this );
	slowMotionBox->addItem( "1 x", QVariant( 1 ) );
	slowMotionBox->addItem( "1/2 x", QVariant( 2 ) );
	slowMotionBox->addItem( "1/4 x", QVariant( 4 ) );
	slowMotionBox->addItem( "1/8 x", QVariant( 8 ) );
	slowMotionBox->setCurrentIndex( 0 );
	connect( slowMotionBox, SIGNAL( activated( int ) ), SLOT( updateSlowMotion( int ) ) );

	QBoxLayout *layout = new QHBoxLayout;
	layout->setMargin( 0 );
	layout->setSpacing( 2 );
	layout->addWidget( previousButton );
	layout->addWidget( playButton );
	layout->addWidget( stopButton );
	layout->addWidget( nextButton );
	layout->addWidget( label );
	layout->addWidget( slider );
	layout->addWidget( loopButton );
	layout->addWidget( slowMotionBox );
	setLayout( layout );
}

void QPlayControl::setRange( int min, int max )
{
	slider->setRange( min, max );
}

void QPlayControl::setTime( double time )
{
	currentTime = time;

	slider->blockSignals( true );

	slider->setValue( int( currentTime * 1000 ) );
	label->setText( QString().sprintf( "%.3f", currentTime ) );

	slider->blockSignals( false );
}

bool QPlayControl::getLoop()
{
	return loopButton->isChecked();
}

void QPlayControl::setLoop( bool loop )
{

}

void QPlayControl::playClicked()
{

}

void QPlayControl::updateSlowMotion( int idx )
{
	emit slowMotionChanged( slowMotionBox->itemData( idx ).toInt() );
}

void QPlayControl::updateSlider( int value )
{
	setTime( value / 1000.0 );
	emit timeChanged( currentTime );
	emit sliderChanged( value );
}
