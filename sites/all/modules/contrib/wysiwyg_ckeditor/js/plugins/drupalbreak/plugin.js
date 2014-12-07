/**
 * @file Drupal teaser break plugin.
 *
 * This file is a placeholder until we can implements true Drupal-side dialog
 * handling, pending the patch at 
 *
 * Note that this file follows coding standards for the CKEditor project rather
 * than traditional Drupal coding standards for JavaScript. This keeps
 * compatibility high when adapting code between the built-in plugins and the
 * custom plugins that Drupal provides.
 *
 * @see http://dev.ckeditor.com/wiki/CodingStyle
 */
( function()
{

CKEDITOR.plugins.add( 'drupalbreak',
{
	requires  : [ 'fakeobjects' ],

	init : function( editor )
	{
		// Register the toolbar buttons.
		editor.ui.addButton( 'DrupalBreak',
		{
			label : Drupal.t('Insert Teaser Break'),
			icon : Drupal.settings.basePath + Drupal.settings.ckeditor.modulePath + '/images/buttons/pagebreak' + (editor.lang.dir === 'rtl' ? '-rtl' : '') + '.png',
			command : 'drupalbreak'
		});

		editor.addCommand( 'drupalbreak',
		{
			exec : function()
			{
				// There should be only one <!--break--> in document. So, look
				// for an image with class "cke_drupal_break" (the fake element).
				var images = editor.document.getElementsByTag( 'img' );
				for ( var i = 0, len = images.count() ; i < len ; i++ )
				{
					var img = images.getItem( i );
					if ( img.hasClass( 'cke_drupal_break' ) )
					{
						if ( confirm( Drupal.t( 'The document already contains a teaser break. Do you want to proceed by removing it first?' ) ) )
						{
							img.remove();
							break;
						}
						else
							return;
					}
				}

				insertComment( 'break' );
			}
		} );

		editor.ui.addButton( 'DrupalPageBreak',
		{
			label : Drupal.t( 'Insert Page Break' ),
			icon : this.path + 'images/drupalpagebreak.png',
			command : 'drupalpagebreak'
		});

		editor.addCommand( 'drupalpagebreak',
		{
			exec : function()
			{
				insertComment( 'pagebreak' );
			}
		} );

		// This function effectively inserts the comment into the editor.
		function insertComment( text )
		{
			// Create the fake element that will be inserted into the document.
			// The trick is declaring it as an <hr>, so it will behave like a
			// block element (and in effect it behaves much like an <hr>).
			if ( !CKEDITOR.dom.comment.prototype.getAttribute ) {
				CKEDITOR.dom.comment.prototype.getAttribute = function() {
					return '';
				};
				CKEDITOR.dom.comment.prototype.attributes = {
					align : ''
				};
			}
			var fakeElement = editor.createFakeElement( new CKEDITOR.dom.comment( text ), 'cke_drupal_' + text, 'hr' );

			// This is the trick part. We can't use editor.insertElement()
			// because we need to put the comment directly at <body> level.
			// We need to do range manipulation for that.

			// Get a DOM range from the current selection.
			var range = editor.getSelection().getRanges()[0],
			elementsPath = new CKEDITOR.dom.elementPath( range.getCommonAncestor( true ) ),
			element = ( elementsPath.block && elementsPath.block.getParent() ) || elementsPath.blockLimit,
			hasMoved;

			// If we're not in <body> go moving the position to after the
			// elements until reaching it. This may happen when inside tables,
			// lists, blockquotes, etc.
			while ( element && element.getName() != 'body' )
			{
				range.moveToPosition( element, CKEDITOR.POSITION_AFTER_END );
				hasMoved = 1;
				element = element.getParent();
			}

			// Split the current block.
			if ( !hasMoved )
				range.splitBlock( 'p' );

			// Insert the fake element into the document.
			range.insertNode( fakeElement );
		
			// Now, we move the selection to the best possible place following
			// our fake element.
			var next = fakeElement;
			while ( ( next = next.getNext() ) && !range.moveToElementEditStart( next ) )
			{}

			range.select();
		}
	},

	afterInit : function( editor )
	{
		// Adds the comment processing rules to the data filter, so comments
		// are replaced by fake elements.
		editor.dataProcessor.dataFilter.addRules(
		{
			comment : function( value )
			{
				if ( !CKEDITOR.htmlParser.comment.prototype.getAttribute ) {
					CKEDITOR.htmlParser.comment.prototype.getAttribute = function() {
						return '';
					};
					CKEDITOR.htmlParser.comment.prototype.attributes = {
						align : ''
					};
				}
			
				if ( value == 'break' || value == 'pagebreak' )
					return editor.createFakeParserElement( new CKEDITOR.htmlParser.comment( value ), 'cke_drupal_' + value, 'hr' );

				return value;
			}
		});
	}
});

})();
