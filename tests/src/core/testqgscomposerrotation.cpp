/***************************************************************************
                         testqgscomposerrotation.cpp
                         ----------------------
    begin                : April 2013
    copyright            : (C) 2013 by Marco Hugentobler, Nyall Dawson
    email                : nyall dot dawson at gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsapplication.h"
#include "qgscomposition.h"
#include "qgsmultirenderchecker.h"
#include "qgscomposershape.h"
#include "qgscomposermap.h"
#include "qgscomposerlabel.h"
#include "qgsmultibandcolorrenderer.h"
#include "qgsrasterlayer.h"
#include "qgsmaplayerregistry.h"
#include "qgsfontutils.h"
#include "qgsrasterdataprovider.h"
#include <QObject>
#include <QtTest/QtTest>
#include <QColor>
#include <QPainter>

class TestQgsComposerRotation : public QObject
{
    Q_OBJECT

  public:
    TestQgsComposerRotation()
        : mComposition( 0 )
        , mComposerRect( 0 )
        , mComposerLabel( 0 )
        , mComposerMap( 0 )
        , mMapSettings( 0 )
        , mRasterLayer( 0 )
    {}

  private slots:
    void initTestCase();// will be called before the first testfunction is executed.
    void cleanupTestCase();// will be called after the last testfunction was executed.
    void init();// will be called before each testfunction is executed.
    void cleanup();// will be called after every testfunction.

    // All old (deprecated) methods tests disabled (we have enough troubles to maintain not deprecated)
    // Label tests disabled because are platform dependent (font)
    void shapeRotation(); //test if composer shape rotation is functioning
    //void oldShapeRotationApi(); //test if old deprecated composer shape rotation api is functioning
    void labelRotation(); //test if composer label rotation is functioning
    //void oldLabelRotationApi(); //test if old deprectated composer label rotation api is functioning
    void mapRotation(); //test if composer map mapRotation is functioning
    void mapItemRotation(); //test if composer map item rotation is functioning
    //void oldMapRotationApi(); //test if old deprectated composer map rotation api is functioning

  private:
    QgsComposition* mComposition;
    QgsComposerShape* mComposerRect;
    QgsComposerLabel* mComposerLabel;
    QgsComposerMap* mComposerMap;
    QgsMapSettings *mMapSettings;
    QgsRasterLayer* mRasterLayer;
    QString mReport;
};

void TestQgsComposerRotation::initTestCase()
{
  QgsApplication::init();
  QgsApplication::initQgis();

  mMapSettings = new QgsMapSettings();

  //create maplayers from testdata and add to layer registry
  QFileInfo rasterFileInfo( QString( TEST_DATA_DIR ) + "/rgb256x256.png" );
  mRasterLayer = new QgsRasterLayer( rasterFileInfo.filePath(),
                                     rasterFileInfo.completeBaseName() );
  QgsMultiBandColorRenderer* rasterRenderer = new QgsMultiBandColorRenderer( mRasterLayer->dataProvider(), 1, 2, 3 );
  mRasterLayer->setRenderer( rasterRenderer );

  QgsMapLayerRegistry::instance()->addMapLayers( QList<QgsMapLayer*>() << mRasterLayer );

  mMapSettings->setLayers( QStringList() << mRasterLayer->id() );
  mMapSettings->setCrsTransformEnabled( false );

  mComposition = new QgsComposition( *mMapSettings );
  mComposition->setPaperSize( 297, 210 ); //A4 landscape

  mComposerRect = new QgsComposerShape( 70, 70, 150, 100, mComposition );
  mComposerRect->setShapeType( QgsComposerShape::Rectangle );
  mComposerRect->setBackgroundColor( QColor::fromRgb( 255, 150, 0 ) );

  mComposerLabel = new QgsComposerLabel( mComposition );
  mComposerLabel->setText( "test label" );
  mComposerLabel->setFont( QgsFontUtils::getStandardTestFont() );
  mComposerLabel->setPos( 70, 70 );
  mComposerLabel->adjustSizeToText();
  mComposerLabel->setBackgroundColor( QColor::fromRgb( 255, 150, 0 ) );
  mComposerLabel->setBackgroundEnabled( true );

  mReport = "<h1>Composer Rotation Tests</h1>\n";
}

void TestQgsComposerRotation::cleanupTestCase()
{
  if ( mComposerMap )
  {
    mComposition->removeItem( mComposerMap );
    delete mComposerMap;
  }

  delete mComposerLabel;
  delete mComposerRect;
  delete mComposition;
  delete mMapSettings;

  QString myReportFile = QDir::tempPath() + "/qgistest.html";
  QFile myFile( myReportFile );
  if ( myFile.open( QIODevice::WriteOnly | QIODevice::Append ) )
  {
    QTextStream myQTextStream( &myFile );
    myQTextStream << mReport;
    myFile.close();
  }

  QgsApplication::exitQgis();
}

void TestQgsComposerRotation::init()
{

}

void TestQgsComposerRotation::cleanup()
{

}

void TestQgsComposerRotation::shapeRotation()
{
  mComposition->addComposerShape( mComposerRect );

  mComposerRect->setItemRotation( 45, true );

  QgsCompositionChecker checker( "composerrotation_shape", mComposition );
  checker.setControlPathPrefix( "composer_items" );
  QVERIFY( checker.testComposition( mReport ) );

  mComposition->removeItem( mComposerRect );
  mComposerRect->setItemRotation( 0, true );
}

#if 0
void TestQgsComposerRotation::oldShapeRotationApi()
{
  //test old style deprecated rotation api - remove after 2.0 series
  mComposition->addComposerShape( mComposerRect );

  mComposerRect->setRotation( 45 );

  QgsCompositionChecker checker( "composerrotation_shape_oldapi", mComposition );
  checker.setControlPathPrefix( "composer_items" );
  QVERIFY( checker.testComposition( mReport ) );

  mComposition->removeItem( mComposerRect );
}
#endif

void TestQgsComposerRotation::labelRotation()
{
  mComposition->addComposerLabel( mComposerLabel );
  mComposerLabel->setItemRotation( 135, true );

  QgsCompositionChecker checker( "composerrotation_label", mComposition );
  checker.setControlPathPrefix( "composer_items" );
  QVERIFY( checker.testComposition( mReport, 0, 0 ) );
}

#if 0
void TestQgsComposerRotation::oldLabelRotationApi()
{
  //test old style deprecated rotation api - remove test after 2.0 series
  mComposition->addComposerLabel( mComposerLabel );

  mComposerLabel->setRotation( 135 );

  QgsCompositionChecker checker( "composerrotation_label_oldapi", mComposition );
  checker.setControlPathPrefix( "composer_items" );
  QVERIFY( checker.testComposition( mReport ) );

  mComposition->removeItem( mComposerLabel );
}
#endif

void TestQgsComposerRotation::mapRotation()
{
  // cleanup after labelRotation()
  mComposition->removeItem( mComposerLabel );
  mComposerLabel->setItemRotation( 0, true );

  //test map rotation
  mComposerMap = new QgsComposerMap( mComposition, 20, 20, 100, 50 );
  mComposerMap->setFrameEnabled( true );
  mComposition->addItem( mComposerMap );
  mComposerMap->setNewExtent( QgsRectangle( 0, -192, 256, -64 ) );
  mComposerMap->setMapRotation( 90 );

  QgsCompositionChecker checker( "composerrotation_maprotation", mComposition );
  checker.setControlPathPrefix( "composer_items" );
  QVERIFY( checker.testComposition( mReport, 0, 200 ) );
}

void TestQgsComposerRotation::mapItemRotation()
{
  // cleanup after mapRotation()
  mComposition->removeItem( mComposerMap );
  delete mComposerMap;

  //test map item rotation
  mComposerMap = new QgsComposerMap( mComposition, 20, 50, 100, 50 );
  mComposerMap->setFrameEnabled( true );
  mComposition->addItem( mComposerMap );
  mComposerMap->setNewExtent( QgsRectangle( 0, -192, 256, -64 ) );
  mComposerMap->setItemRotation( 90, true );

  QgsCompositionChecker checker( "composerrotation_mapitemrotation", mComposition );
  checker.setControlPathPrefix( "composer_items" );
  QVERIFY( checker.testComposition( mReport ) );
}

QTEST_MAIN( TestQgsComposerRotation )
#include "testqgscomposerrotation.moc"
