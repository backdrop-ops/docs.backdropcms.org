/**
 * @file Drupal Caption plugin.
 *
 * Note that this file follows coding standards for the CKEditor project rather
 * than traditional Drupal coding standards for JavaScript. This keeps
 * compatibility high when adapting code between the built-in plugins and the
 * custom plugins that Drupal provides.
 *
 * @see http://dev.ckeditor.com/wiki/CodingStyle
 */

(function()
{

CKEDITOR.plugins.add( 'drupalcaption', 
{
	requires: [ 'drupalimage' ],

	init : function( editor )
	{

		if ( editor.blockless )
		{
			return;
		}

		CKEDITOR.on( 'dialogDefinition', function( e )
		{
			if ( e.data.name == 'image' && !e.data.definition.getContents( 'caption' ) )
			{
				var infoTab = e.data.definition.getContents('info');
				infoTab.add( {
					type : 'text',
					id : 'captionText',
					label : 'Caption',
					onHide : function()
					{
						this.enable();
					},
					setup : function( type, element )
					{
						if ( type == 1 )
						{
							var el = this.getDialog().cleanImageElement,
								parent = el && el.getAscendant( 'div' ),
								caption;
							if ( parent && ( caption = parent.getParent() ) && caption.is( 'div' ) && caption.hasClass( 'caption' ) )
							{
								var next = el.getNext(),
									bogus = el.getParent().getBogus();
								if ( next )
								{
									this.setValue( next.getHtml() );
									if ( ( next = next.getNext() ) && !next.equals( bogus ) )
										this.disable();
								}
								
								var select = this.getDialog().getContentElement( 'caption', 'captionStyle' );
								select && select.setValue( caption.hasClass( 'caption-right' ) ? 'right' :
											caption.hasClass( 'caption-left' ) ? 'left' :
												caption.hasClass( 'caption-center' ) ? 'center' : '' );

								this.captionElement = caption;
							}
						}
					},
					commit : function( type, element )
					{
						if ( type == 1 )
						{
							var caption = this.captionElement,
								sel = editor.getSelection(),
								selected = sel && sel.getSelectedElement();

							if ( selected && !selected.is( 'img' ) )
								selected = null;

							if ( !caption )
							{
								if ( !this.getValue() )
									return null;

								caption = CKEDITOR.dom.element.createFromHtml( '<div class="caption"><div class="caption-inner"><div class="caption-text"></div></div></div>' );
								if ( this.linkElement )
								{
									caption.insertBefore( this.linkElement );
									this.linkElement.appendTo( caption.getFirst() );
								}
								else
								{
									if ( selected )
									{
										caption.insertBefore( selected );
										selected.move( caption.getFirst(), true );
									}
									else
									{
										editor.insertElement( caption );
										var imageElement = this.getDialog().imageElement;
										setTimeout( function(){ imageElement.move( caption.getFirst(), true ); }, 0 );
									}
								}
							}

							if ( this.isEnabled() )
							{
								if ( !this.getValue() )
								{
									if ( this.linkElement )
										this.linkElement.insertAfter( caption );
									else
										selected.insertAfter( caption );
									caption.remove();
								}
								else if ( this.isChanged() )
								{
									var inner = caption.getFirst();
									if ( inner )
									{
										var bogus = inner ? inner.getBogus() : null;
										bogus && bogus.remove();
										inner.getLast().setHtml( this.getValue() );
									}
								}
							}

							var select = this.getDialog().getContentElement( 'caption', 'captionStyle' );
							if ( select && ( !select.getValue() || !caption.hasClass( 'caption-' + select.getValue() ) ) )
							{
								caption.removeClass( 'caption-right' );
								caption.removeClass( 'caption-left' );
								caption.removeClass( 'caption-center' );
								select.getValue() && caption.addClass( 'caption-' + select.getValue() );
							}
						}
					}
				}, 'txtAlt' );

				// Place the field after the ALT text.
				var altField = infoTab.get('txtAlt')
				infoTab.remove('txtAlt');
				infoTab.add(altField, 'captionText');
			}
		});

		if ( editor.addMenuItem )
		{
			editor.addCommand( 'removecaption',
			{
				modes : { wysiwyg : 1 },
				exec : function( editor )
				{
					var sel = editor.getSelection(),
						caption = sel && sel.getStartElement();
					if ( !caption || !( caption = getSelectedCaption( caption ) ) )
						return null;
					var inner = caption.getFirst( function( node )
					{
						return node.type == CKEDITOR.NODE_ELEMENT && node.is( 'div' ) && node.hasClass( 'caption-inner' );
					});
					caption.remove( true );
					inner.getFirst( function( node )
					{
						return node.type == CKEDITOR.NODE_ELEMENT && node.is( 'img' );
					}).clone().insertAfter( inner );
					inner.remove();
				}
			});
			editor.addMenuItem( 'removeCaption',
			{
				label : 'Remove Caption',
				command : 'removecaption',
				group : 'div',
				order : 1
			});
			editor.contextMenu.addListener( function( element, selection )
			{
				if ( !element || element.isReadOnly() )
					return null;

				if ( getSelectedCaption( element ) )
					return { removeCaption : CKEDITOR.TRISTATE_OFF };
			});
		}

		if ( editor.keystrokeHandler )
		{
			var ks = editor.keystrokeHandler.keystrokes,
				handler = ks[ 13 ];
			editor.addCommand( 'captionEnter',
			{
				modes : { wysiwyg : 1 },
				editorFocus : false,
				exec : function( editor )
				{
					var sel = editor.getSelection(),
						caption = sel && sel.getStartElement(),
						range = sel && sel.getRanges()[ 0 ];
					if ( caption && ( caption = getSelectedCaption( caption ) ) )
					{
						CKEDITOR.env.ie && ( CKEDITOR.env.version < 9 ) && caption.getParent().focus();
						range.moveToPosition( caption, CKEDITOR.POSITION_AFTER_END );
						range.select();
					}
					return editor.execCommand( handler );
				}
			});
			ks[ 13 ] = 'captionEnter';
		}
	},

	afterInit : function( editor )
	{
		if ( editor.blockless )
		{
			return;
		}

		// Make the image button select the image when inside a captioned area.
		var imageCommand = editor.getCommand( 'image' );
		if ( imageCommand )
		{
			imageCommand.on( 'exec', function( evt )
				{
						sel = editor.getSelection(),
						element = sel && sel.getStartElement();
						var image = getSelectedCaptionImage( element );
						if ( image )
						{
							sel.selectElement( image );
						}
				// Give a weight of 7 so it comes before the normal image execution
				}, 7);
		}

		// Customize the behavior of the alignment commands for captions.
		setupAlignCommand( 'left' );
		setupAlignCommand( 'right' );
		setupAlignCommand( 'center' );
		setupAlignCommand( 'block' );

		function setupAlignCommand( value )
		{
			var command = editor.getCommand( 'justify' + value );
			if ( command )
			{
				command.on( 'exec', function( evt )
					{
						sel = editor.getSelection(),
						element = sel && sel.getStartElement();
						var caption = getSelectedCaption( element ), align;
						if ( caption )
						{
							// Remove the state of the previous alignment button.
							previousAlignment = getCaptionAlignment( caption );
							if (previousAlignment)
							{
								var previousCommand = editor.getCommand( 'justify' + previousAlignment );
								if ( previousCommand.state === CKEDITOR.TRISTATE_ON)
									previousCommand.setState(CKEDITOR.TRISTATE_OFF);
							}
							// Set alignment and activate the current alignment button.
							if ( setCaptionAlignment( caption, value ) ) {
								command.setState(CKEDITOR.TRISTATE_ON);
							}

							// Cancel upstream changes unless displaying as block.
							if ( !( value === 'block' && element.is('img') ) )
								evt.cancel();
						}
					// Register this event at position 8 (default is 10) so that it fires
					// before image alignment.
					}, null, null, 8);

				command.on( 'refresh', function( evt )
					{
						var sel = editor.getSelection(),
						element = sel && sel.getStartElement();
						var caption = getSelectedCaption( element ), align;
						if ( caption )
						{
							align = getCaptionAlignment( caption );

							this.setState( ( align == value ) ? CKEDITOR.TRISTATE_ON : CKEDITOR.TRISTATE_OFF );

							// If an image is not selected in the caption, full justify is not
							// supported.
							if ( value === 'block' && !element.is('img') )
								this.setState( CKEDITOR.TRISTATE_DISABLED );

							// Don't allow further handlers to adjust the button state.
							if ( !( value === 'block' && element.is('img') ) )
								evt.cancel();
						}
					}, null, null, 8);
			}
		}
	}
});


function getCaptionAlignment( element )
{
  var align = false;
	var justifyClasses = [ 'caption-left', 'caption-center', 'caption-right' ];
	var justifyNames = [ 'left', 'center', 'right' ];
	for (var classPosition = 0; classPosition < 3; classPosition++) {
		if (element.hasClass(justifyClasses[classPosition]))
			align = justifyNames[classPosition];
	}

	return align;
}

function getSelectedCaption( element )
{
	while ( element && !element.hasClass( 'caption' ) )
		element = element.getAscendant( 'div' );
	return element && element.hasClass( 'caption' ) ? element : null;
}

function getSelectedCaptionImage( element )
{
	var caption = getSelectedCaption( element );
	if ( caption )
	{
		var nodeList = caption.$.getElementsByTagName( 'img' );
		if ( nodeList.length )
		{
			return new CKEDITOR.dom.element( nodeList[0] );
		}
	}
}

function setCaptionAlignment( wrapper, value )
{
	// Check existing alignment.
	var align = getCaptionAlignment( wrapper );

	// Remove existing alignment.
	wrapper.removeClass( 'caption-' + align );

	// If changing the alignment to a new value, set the new style.
	if ( value && align !== value && value !== 'block' )
		wrapper.addClass( 'caption-' + value );

	return align !== value && value !== 'block';
}

})();
