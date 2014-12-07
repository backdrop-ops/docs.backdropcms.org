/**
 * @file Drupal Image plugin.
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

CKEDITOR.plugins.add( 'drupalimage',
{
	// @todo: Remove dependency on normal image dialog, build Drupal-based dialog.
	requires: [ 'dialog' ],

	init : function( editor )
	{
		var pluginName = 'drupalimage';

		// Register the normal image dialog.
		CKEDITOR.dialog.add( 'image', Drupal.settings.basePath + Drupal.settings.ckeditor.modulePath + '/lib/ckeditor/plugins/image/dialogs/image.js' );

		// Register the toolbar button.
		editor.ui.addButton( 'DrupalImage',
			{
				label : editor.lang.common.image,
				command : 'image',
				icon : Drupal.settings.basePath + Drupal.settings.ckeditor.modulePath + '/images/buttons/image.png',
			});

		// Register the image command.
		editor.addCommand( 'image', new CKEDITOR.dialogCommand( 'image' ) );

		// Double clicking an image opens its properties.
		editor.on( 'doubleclick', function( evt )
			{
				var element = evt.data.element;

				if ( element.is( 'img' ) && !element.data( 'cke-realelement' ) && !element.isReadOnly() )
					evt.data.dialog = 'image';
			});

		// If the "menu" plugin is loaded, register the menu items.
		if ( editor.addMenuItems )
		{
			editor.addMenuItems(
				{
					image :
					{
						label : editor.lang.image.menu,
						command : 'image',
						group : 'image'
					}
				});
		}

		// If the "contextmenu" plugin is loaded, register the listeners.
		if ( editor.contextMenu )
		{
			editor.contextMenu.addListener( function( element, selection )
				{
					if ( getSelectedImage( editor, element ) )
						return { image : CKEDITOR.TRISTATE_OFF };
				});
		}

		// Customize the built-in dialog for images.
		CKEDITOR.on( 'dialogDefinition', function( evt ) {
			if ( evt.data.name == 'image' )
			{
				var infoPane = evt.data.definition.getContents('info');

				// Remove unneeded options that write style attributes.
				infoPane.remove('txtHSpace');
				infoPane.remove('txtVSpace');
				infoPane.remove('txtBorder');

				// Add option for center alignment if supported.
				var alignOption = infoPane.get('cmbAlign');
				if ( CKEDITOR.config.drupalimage_justifyClasses )
					alignOption.items.push([ editor.lang.common.alignCenter, 'center' ]);

				// Adjust the alignment options to use our alignment classes.
				alignOption.setup = function ( type, element ) {
					// Set alignment value based on active alignment state.
					var value = '';
					var possibleButtons = [ 'left', 'center', 'right', 'block' ];
					for (var n = 0; n < 4; n++) {
						var currentCommand = editor.getCommand('justify' + possibleButtons[n]);
						if ( currentCommand && currentCommand.state === CKEDITOR.TRISTATE_ON )
						  value = possibleButtons[n];
					}
					if ( value )
						this.setValue( value );
					this.setupValue = value;
				}
				alignOption.commit = function( type, element, internalCommit ) {
					// Constants from image plugin dialog.js.
					var IMAGE = 1, LINK = 2, PREVIEW = 4, CLEANUP = 8;
					var value = this.getValue();
					switch ( type )
					{
						case IMAGE:
							if ( value !== this.setupValue )
							{
								if ( value )
								{
									var alignCommand = editor.getCommand('justify' + value );
									if ( alignCommand.state === CKEDITOR.TRISTATE_OFF )
										alignCommand.exec();
								}
								else if ( this.setupValue )
								{
									var alignCommand = editor.getCommand('justify' + this.setupValue );
									if ( alignCommand.state === CKEDITOR.TRISTATE_ON )
										alignCommand.exec();
								}
							}
							break;
						case PREVIEW:
							setImageAlignment( element, value );
							break;
						case CLEANUP:
							setImageAlignment( element, false );
							break;
					}
				}
			}
		});

	},
	afterInit : function( editor )
	{
		// Use normal height/width attributes for images instead of styles (#5547).
		// We may be able to remove this code once Drupal provides its own modal
		// dialogs for manipulating images, but resizing an image via resize handles
		// may make it necessary still.
		var dataProcessor = editor.dataProcessor;
		var htmlFilter = dataProcessor && dataProcessor.htmlFilter;
		htmlFilter.addRules(
			{
				elements : 
				{
					img : function( element )
					{
						var style = element.attributes.style, match, width, height;
						if ( style )
						{
							// Get the width from the style.
							match = /(?:^|\s)width\s*:\s*(\d+)px/i.exec( style ),
							width = match && match[1];
							// Get the height from the style.
							match = /(?:^|\s)height\s*:\s*(\d+)px/i.exec( style );
							height = match && match[1];
							if ( width && width.length )
							{
								style = style.replace( /(?:^|\s)width\s*:\s*(\d+)px;?/i , '' );
								element.attributes.width = width;
							}
							if ( height && height.length )
							{
								style = style.replace( /(?:^|\s)height\s*:\s*(\d+)px;?/i , '' );
								element.attributes.height = height;
							}
							if (style)
								element.attributes.style = style;
							else
								delete element.attributes.style;
						}
					}
				}
			}
		);

		// Customize the behavior of the alignment commands. (#7430)
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
						var img = getSelectedImage( editor ), align;
						if ( img )
						{
							// Remove the state of the previous alignment button.
							previousAlignment = getImageAlignment( img );
							if (previousAlignment)
							{
								var previousCommand = editor.getCommand( 'justify' + previousAlignment );
								if ( previousCommand.state === CKEDITOR.TRISTATE_ON)
									previousCommand.setState(CKEDITOR.TRISTATE_OFF);
							}
							// Set alignment and activate the current alignment button.
							if ( setImageAlignment( img, value ) ) {
								command.setState(CKEDITOR.TRISTATE_ON);
							}
							evt.cancel();
						}
					});

				command.on( 'refresh', function( evt )
					{
						var img = getSelectedImage( editor ), align;
						if ( img )
						{
							align = getImageAlignment( img );

							this.setState(
								( align == value ) ? CKEDITOR.TRISTATE_ON :
								( CKEDITOR.config.drupalimage_justifyClasses || value == 'right' || value == 'left' ) ? CKEDITOR.TRISTATE_OFF :
								CKEDITOR.TRISTATE_DISABLED );

							evt.cancel();
						}
					});
			}
		}
	}
});

function getSelectedImage( editor, element )
{
	if ( !element )
	{
		var sel = editor.getSelection();
		var selectedText = sel.getSelectedText().replace(/^\s\s*/, '').replace(/\s\s*$/, '');
		var isElement = sel.getType() === CKEDITOR.SELECTION_ELEMENT;
		var isEmptySelection = sel.getType() === CKEDITOR.SELECTION_TEXT && selectedText.length === 0;
		element = ( isElement || isEmptySelection ) && sel.getSelectedElement();
	}

	if ( element && element.is( 'img' ) && !element.data( 'cke-realelement' ) && !element.isReadOnly() )
		return element;
}

function getImageAlignment( element )
{
	// Get alignment based on precedence of display used in browsers:
	// 1) inline style, 2) class style, then 3) align attribute.
	var align = element.getStyle( 'float' );

	if ( align == 'inherit' || align == 'none' )
		align = 0;

	if ( !align && CKEDITOR.config.drupalimage_justifyClasses ) {
		var justifyClasses = CKEDITOR.config.drupalimage_justifyClasses;
		var justifyNames = [ 'left', 'center', 'right', 'block' ];
		for (var classPosition = 0; classPosition < 4; classPosition++) {
			if (element.hasClass(justifyClasses[classPosition]))
				align = justifyNames[classPosition];
		}
	}

	if ( !align )
		align = element.getAttribute( 'align' );

	return align;
}

function setImageAlignment( img, value )
{
	align = getImageAlignment( img );
	if ( CKEDITOR.config.drupalimage_justifyClasses )
	{
		var justifyClasses = CKEDITOR.config.drupalimage_justifyClasses;
		var justifyNames = [ 'left', 'center', 'right', 'block' ];
		var justifyOldPosition = CKEDITOR.tools.indexOf( justifyNames, align );
		var justifyOldClassName = justifyOldPosition === -1 ? null : justifyClasses[justifyOldPosition];
		var justifyNewPosition = CKEDITOR.tools.indexOf( justifyNames, value );
		var justifyNewClassName = justifyNewPosition === -1 ? null : justifyClasses[justifyNewPosition];
	}

	// If this image is already aligned, remove existing alignment.
	if ( align )
	{
		img.removeStyle( 'float' );
		img.removeAttribute( 'align' );
		if ( justifyOldClassName )
			img.removeClass( justifyOldClassName );
	}

	// If changing the alignment to a new value, set the new style.
	if ( value && align !== value )
	{
		if ( justifyNewClassName )
			img.addClass( justifyNewClassName );
		else
			img.setStyle( 'float', value );
	}

	return align !== value;
}

})();

/**
 * List of classes to use for aligning images. Each class will be used when
 * an image is selected and the normal justify toolbar buttons are clicked. The
 * array of classes should contain 4 members, in the following order: left,
 * center, right, justify. If the list of classes is null, CSS style attributes
 * will be used instead.
 *
 * @type Array
 * @default true
 * @example
 * // Disable the list of classes and use styles instead.
 * config.drupalimage_justifyClasses = null;
 */
CKEDITOR.config.drupalimage_justifyClasses = [ 'align-left', 'align-center', 'align-right', 'full-width' ];
