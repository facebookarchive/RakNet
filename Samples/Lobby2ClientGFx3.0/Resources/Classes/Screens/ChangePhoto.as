import flash.external.*;
import gfx.controls.TextInput;
import gfx.controls.Button;
import mx.utils.Delegate;

class Screens.ChangePhoto extends Screen
{	
	private var btnBack:Button;		
	private var btnAcceptPhoto:Button;		
	private var tfUsername:TextField;	
	private var mcProfileImage:MovieClip;
		
	private var mcImageSelector:MovieClip;
	private var mImageList:Array;
	private var mCurrentPage:Number;
	private var mcPageNavigator:PageNavigator;
	private var mSelectedIndex:Number;
	
	public static var cFirstImageX:Number = 0;
	public static var cFirstImageY:Number = -120;
	public static var cImageDeltaX:Number = 40;
	public static var cImageDeltaY:Number = 40;
	public static var cImagesPerRow:Number = 5;
	public static var cImagesPerPage:Number = 15;
	
	public function ChangePhoto() 
	{
		ConsoleWindow.Trace("Constructing ChangePhoto");
		
		mScreenId = ScreenID.CHANGE_PHOTO;
		mScreenTabId = ScreenTab.ID_PROFILE;
		
		mImageList = new Array();
		mCurrentPage = 0;
		mSelectedIndex = -1;
	}
	
	public function VOnFinishedLoading():Void
	{									
		//Add click event for buttons
		btnBack.addEventListener("click", this, "Back");
		btnAcceptPhoto.addEventListener("click", this, "AcceptPhoto");
						
		super.VOnFinishedLoading();
	}
	
	public function OnShow():Void
	{
		mcImageSelector._visible = false;
		tfUsername.text = LobbyInterface.Instance.GetUsername();
				
		if ( !mcProfileImage.mcImageContainer.mcImage )
		{
			var imageIndex:Number = LobbyInterface.Instance.GetProfileImageIndex();
			mcProfileImage.attachMovie( "ProfileImage" + imageIndex, "mcImage", mcProfileImage.getNextHighestDepth() );			
		}
		
		mSelectedIndex = -1;
		mcImageSelector._visible = false;
		
		if ( mImageList.length == 0 )
		{
			AddImages();
		}
		else
		{
			ShowPage( mCurrentPage );			
		}
	}
	
	public function AddImages():Void
	{
		for ( var i:Number = 1; i <= 35; i++ )
		{
			var profileImage:MovieClip = attachMovie( "ProfileImage1", "ProfileImage" + i, getNextHighestDepth() );
			profileImage._visible = false;
			profileImage._width = mcImageSelector._width;
			profileImage._height = mcImageSelector._height;
			ConsoleWindow.Trace("Adding image " + + profileImage);
			profileImage.tfTemp.text = mImageList.length;
			mImageList.push( profileImage );
			
			var self:ChangePhoto = this;
			profileImage.onPress = function() { self.OnSelectImage( this );	}
		}
		
		mcImageSelector.swapDepths( getNextHighestDepth() );
		mcPageNavigator.AddCallbacks( Delegate.create( this, NextPage ), Delegate.create( this, PreviousPage ) );
		mcPageNavigator.SetTotalPages( Math.ceil( mImageList.length / cImagesPerPage ) );
				
		ShowPage( 1 );
	}
	
	public function OnSelectImage( _profileImage:MovieClip ):Void
	{
		var index:Number = Number( _profileImage._name.substring( 12, _profileImage._name.length ) ) - 1;
		
		ConsoleWindow.Trace("Selected: " + _profileImage + ", index = " + index);
		if ( index != mSelectedIndex )
		{
			mSelectedIndex = index;
			mcImageSelector._x = _profileImage._x;
			mcImageSelector._y = _profileImage._y;
			mcImageSelector._visible = true;
		}
	}
	
	public function NextPage():Void 
	{
		ShowPage( mCurrentPage + 1);
	}
	
	public function PreviousPage():Void
	{
		ShowPage( mCurrentPage - 1 );
	}
	
	public function ShowPage( _page:Number ):Void
	{
		if ( ( _page - 1 ) * cImagesPerPage <= mImageList.length && _page != mCurrentPage )
		{
			if ( mCurrentPage > 0 )
			{
				HidePage( mCurrentPage );
			}
			ConsoleWindow.Trace("Showing page " + _page); 
			mCurrentPage = _page;
			TogglePageVisibility( mCurrentPage, true );			
			
			//if user selected an image from this new page, we show the selector again at that image
			if ( mSelectedIndex >= ( _page - 1 ) * cImagesPerPage && mSelectedIndex < _page * cImagesPerPage )
			{
				mcImageSelector._visible = true;
			}
		}
	}
	
	public function HidePage( _page:Number ):Void
	{
		if ( ( _page-1 ) * cImagesPerPage <= mImageList.length )
		{
			mcImageSelector._visible = false;
			ConsoleWindow.Trace("Hiding page " + _page); 
			TogglePageVisibility( _page, false );
		}
	}
	
	private function TogglePageVisibility( _page:Number, _visibleState:Boolean ):Void
	{
		var startIndex:Number = _page * cImagesPerPage - ( cImagesPerPage );
		var endIndex:Number = startIndex + cImagesPerPage;
		
		for ( var i:Number = 0; i < cImagesPerPage; i++ )
		{				
			var row:Number = Math.floor( i / cImagesPerRow ) + 1;
			var col:Number = ( i % cImagesPerRow ) + 1;
			var profileImage:MovieClip = mImageList[startIndex+i];
			if ( !profileImage )
			{
				break;
			}
			
			profileImage._visible = _visibleState;
			if ( _visibleState )
			{				
				profileImage._x = cFirstImageX + (col-1) * (profileImage._width + cImageDeltaX);
				profileImage._y = cFirstImageY + (row - 1) * (profileImage._height + cImageDeltaY);
				//ConsoleWindow.Trace("Show profile image: " + profileImage + " at " + row + ", " + col);
			}
			else
			{
				
			}
			
			//ConsoleWindow.Trace("x, y = " + profileImage._x + ", " + profileImage._y);		
		}		
	}
	
	public function Back():Void
	{
		//LobbyInterface.Instance.ShowScreen( ScreenID.PROFILE );
		LobbyInterface.Instance.ShowLastScreen();
	}	
	
	public function AcceptPhoto():Void
	{
		if ( mSelectedIndex >= 0 && mSelectedIndex < mImageList.length )
		{
			//TODO: tell c++ user changed photo to a new index
		}
	}
}