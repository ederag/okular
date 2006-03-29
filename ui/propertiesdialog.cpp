/***************************************************************************
 *   Copyright (C) 2004 by Albert Astals Cid <tsdgeos@terra.es>            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// qt/kde includes
#include <qlayout.h>
#include <qlabel.h>
#include <klocale.h>
#include <k3listview.h>
#include <ksqueezedtextlabel.h>
#include <kglobalsettings.h>

// local includes
#include "propertiesdialog.h"
#include "core/document.h"

PropertiesDialog::PropertiesDialog(QWidget *parent, KPDFDocument *doc)
  : KDialogBase( Tabbed, i18n( "Unknown File" ), Ok, Ok, parent, 0, true, true )
{
  // PROPERTIES
  QFrame *page = addPage(i18n("Properties"));
  QGridLayout *layout = new QGridLayout( page, 2, 2, marginHint(), spacingHint() );

  // get document info, if not present display blank data and a warning
  const DocumentInfo * info = doc->documentInfo();
  if ( !info ) {
    layout->addWidget( new QLabel( i18n( "No document opened." ), page ), 0, 0 );
    return;
  }

  // mime name based on mimetype id
  QString mimeName = info->get( "mimeType" ).section( '/', -1 ).toUpper();
  setCaption( i18n("%1 Properties").arg( mimeName ) );

  QDomElement docElement = info->documentElement();

  int row = 0;
  int valMaxWidth = 100;
  for ( QDomNode node = docElement.firstChild(); !node.isNull(); node = node.nextSibling() ) {
    QDomElement element = node.toElement();

    QString titleString = element.attribute( "title" );
    QString valueString = element.attribute( "value" );
    if ( titleString.isEmpty() || valueString.isEmpty() )
        continue;

    // create labels and layout them
    QLabel *key = new QLabel( i18n( "%1:" ).arg( titleString ), page );
    QLabel *value = new KSqueezedTextLabel( valueString, page );
    layout->addWidget( key, row, 0, Qt::AlignRight );
    layout->addWidget( value, row, 1 );
    row++;

    // refine maximum width of 'value' labels
    valMaxWidth = qMax( valMaxWidth, fontMetrics().width( valueString ) );
  }

  // add the number of pages if the generator hasn't done it already
  QDomNodeList list = docElement.elementsByTagName( "pages" );
  if ( list.count() == 0 ) {
    QLabel *key = new QLabel( i18n( "Pages:" ), page );
    QLabel *value = new QLabel( QString::number( doc->pages() ), page );

    layout->addWidget( key, row, 0 );
    layout->addWidget( value, row, 1 );
  }

  // FONTS
  QVBoxLayout *page2Layout = 0;
  const DocumentFonts * fonts = doc->documentFonts();
  if ( fonts ) {
    // create fonts tab and layout it
    QFrame *page2 = addPage(i18n("Fonts"));
    page2Layout = new QVBoxLayout(page2);
    page2Layout->setMargin(0);
    page2Layout->setSpacing(KDialog::spacingHint());
    // add a klistview with 4 columns
    K3ListView *lv = new K3ListView(page2);
    lv->addColumn( i18n("Name") );
    lv->addColumn( i18n("Type") );
    lv->addColumn( i18n("Embedded") );
    lv->addColumn( i18n("File") );
    page2Layout->addWidget(lv);
    // populate the klistview
    for ( QDomNode node = fonts->documentElement().firstChild(); !node.isNull(); node = node.nextSibling() ) {
      QDomElement e = node.toElement();
      new K3ListViewItem( lv, e.attribute( "Name" ), e.attribute( "Type" ),
                         e.attribute( "Embedded" ), e.attribute( "File" ) );
    }
  }

  // current width: left columnt + right column + dialog borders
  int width = layout->minimumSize().width() + valMaxWidth + 2 * marginHint() + spacingHint() + 30;
  if ( page2Layout )
    width = qMax( width, page2Layout->sizeHint().width() + marginHint() + spacingHint() + 31 );
  // stay inside the 2/3 of the screen width
  QRect screenContainer = KGlobalSettings::desktopGeometry( this );
  width = qMin( width, 2*screenContainer.width()/3 );
  resize(width, 1);
}
