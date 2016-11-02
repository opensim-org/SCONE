#pragma once

#include <QWidget>
#include <QSlider>
#include <QLabel>

class QAbstractButton;
class QAbstractSlider;
class QComboBox;

class QPlayControl : public QWidget
{
	Q_OBJECT

public:
	QPlayControl( QWidget *parent = 0 );
	float slowMotion() const;

	void setRange( int min, int max );
	void setPlaying( bool play );
	void setTime( double time );
	bool getLoop();

public slots:
	void setLoop( bool loop );

signals:
	void play();
	void stop();
	void reset();
	void next();
	void previous();
	void slowMotionChanged( int i );
	void timeChanged( double time );
	void sliderChanged( int );

private slots:
	void playClicked();
	void updateSlowMotion( int );
	void updateSlider( int );

private:
	QAbstractButton *playButton;
	QAbstractButton *stopButton;
	QAbstractButton *nextButton;
	QAbstractButton *previousButton;
	QAbstractButton *loopButton;
	QComboBox *slowMotionBox;
	QSlider *slider;
	QLabel *label;
	double currentTime;
};