#pragma once

#include <QTextEdit>
#include <QPlainTextEdit>

class QCodeTextEdit : public QPlainTextEdit
{
	Q_OBJECT

public:
	QCodeTextEdit( QWidget* parent = 0 );

	void lineNumberAreaPaintEvent( QPaintEvent *event );
	int lineNumberAreaWidth();

public slots:
	void updateLineNumberAreaWidth( int newBlockCount );
	void updateLineNumberArea( const QRect& rect, int dy );

protected:
	void resizeEvent( QResizeEvent *event ) Q_DECL_OVERRIDE;

private:
	QWidget *lineNumberArea;
};
