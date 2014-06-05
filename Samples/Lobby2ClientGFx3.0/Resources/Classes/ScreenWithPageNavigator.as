import mx.utils.Delegate;
import mx.transitions.easing.None;
import mx.transitions.Tween;

// This is an abstract class, and should be extended.
// It only has common functions that can be shared among screens with page navigation functionality
// The extended class still has to setup the list array properly and calling these functions at the right places
class ScreenWithPageNavigator extends Screen
{	
	private var mcPageNavigator:PageNavigator;
	private var mCurrentPage:Number;
	private var mMovieClipList:Array;
	private var mEntriesPerPage:Number = 10;
	private var mFirstEntryX:Number;
	private var mFirstEntryY:Number;
	private var mDeltaY:Number;
				
	public function ScreenWithPageNavigator() 
	{		
		mCurrentPage = 0;
		mScreenId = ScreenID.INVALID_ID;
		mScreenTabId = ScreenTab.ID_INVALID;
		mMovieClipList = new Array();
		
		//extended class should reset these to different values in the constructor
		mFirstEntryX = 0;
		mFirstEntryY = 0;
		mDeltaY = 0;
	}
	
	public function OnShow():Void
	{		
		mcPageNavigator._x = 611;
		mcPageNavigator.AddCallbacks( Delegate.create( this, NextPage ), Delegate.create( this, PreviousPage ) );
	}
	
	public function OnMoveClipListUpdated():Void
	{		
		mcPageNavigator.SetTotalPages( Math.ceil( mMovieClipList.length / mEntriesPerPage ) );
	}
	
	public function GoToPage( _page ):Void
	{
		mcPageNavigator.GoToPage( _page );
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
		if ( ( _page - 1 ) * mEntriesPerPage <= mMovieClipList.length && _page != mCurrentPage )
		{
			if ( mCurrentPage > 0 )
			{
				HidePage( mCurrentPage );
				mCurrentPage = _page;		
			}
			else
			{
				ConsoleWindow.Trace("Showing page " + _page); 
				mCurrentPage = _page;		
				TogglePageVisibility( mCurrentPage, true );
			}				
		}
	}
	
	public function HidePage( _page:Number ):Void
	{
		if ( ( _page-1 ) * mEntriesPerPage <= mMovieClipList.length )
		{
			ConsoleWindow.Trace("Hiding page " + _page); 
			TogglePageVisibility( _page, false );
		}
	}
	
	private function RefreshPage():Void
	{
		//TogglePageVisibility( mCurrentPage, true );
		InstantShowPage();
	}
	
	private function InstantShowPage():Void
	{	
		var startIndex:Number = mCurrentPage * mEntriesPerPage - ( mEntriesPerPage );
		var endIndex:Number = startIndex + mEntriesPerPage;
		
		for ( var i:Number = 0; i < mEntriesPerPage; i++ )
		{				
			var mcEntry:MovieClip = mMovieClipList[startIndex+i];
			if ( !mcEntry )
			{
				break;
			}
			
			mcEntry._visible = true;
			mcEntry._x = mFirstEntryX;
			mcEntry._y = mFirstEntryY + (i) * (mcEntry._height + mDeltaY);	
			OnShowMovieClipEntry( mcEntry );
		}		
	}
	
	private var mLastTime:Number;
	private var mCurEntryIndex:Number;
	private var mFirstEntryIndex:Number;
	private var mEntryVisibleState:Boolean;
	private function TogglePageVisibility( _page:Number, _visibleState:Boolean ):Void
	{
		mcPageNavigator.Disable();
		ConsoleWindow.Trace(this + ".TogglePageVisibility()..." + _visibleState);
		mLastTime = getTimer();
		
		mFirstEntryIndex = _page * mEntriesPerPage - ( mEntriesPerPage );
		mEntryVisibleState = _visibleState;
		
		mCurEntryIndex = 0;
		onEnterFrame = OnTick;	
	}
	
	private function OnTick():Void
	{	
		var now:Number = getTimer();
		var delta:Number = now - mLastTime;
				
		//ConsoleWindow.Trace("OnTick, delta = " + delta);
		//for ( var i:Number = 0; i < mEntriesPerPage; i++ )
		if( delta > 100 )
		{
			ToggleEntryVisibility();
			mLastTime = now;
		}		
	}	
	
	private function ToggleEntryVisibility():Void
	{		
		//ConsoleWindow.Trace(this + ".ToggleEntryVisibility(), mEntryVisibleState = " + mEntryVisibleState);
		if( mCurEntryIndex < mEntriesPerPage )
		{				
			var mcEntry:MovieClip = mMovieClipList[mFirstEntryIndex+mCurEntryIndex];
			if ( !mcEntry )
			{
				ConsoleWindow.Trace("Entry not found.");
				RemoveOnTick();
				return;
			}
			
			mcEntry._visible = true;
			if ( mEntryVisibleState )
			{							
				//ConsoleWindow.Trace("Showing entry: " + mcEntry);
				mcEntry._x = mFirstEntryX;// + 500;
				mcEntry._y = mFirstEntryY + (mCurEntryIndex) * (mcEntry._height + mDeltaY);	
				//new Tween( mcEntry, "_x", None.easeNone, mFirstEntryX-200, mFirstEntryX, .3, true );
				new Tween( mcEntry, "_alpha", None.easeNone, 0, 100, .3, true );
				OnShowMovieClipEntry( mcEntry );
			}
			else
			{				
				var tween_handler:Tween = new Tween( mcEntry, "_alpha", None.easeNone, 100, 0, .3, true );	
				//ConsoleWindow.Trace("Hiding entry: " + mcEntry);	
				//ConsoleWindow.Trace("Hiding tween_handler: " + tween_handler);	
				tween_handler.onMotionFinished = function() 
				{
					this.obj._visible = false;
				}
				//OnHideMovieClipEntry( mcEntry );
			}
			
			mCurEntryIndex++;
			return;
		}		
		
		RemoveOnTick();
	}
	
	public function RemoveOnTick():Void
	{
		onEnterFrame = null;	
		if ( !mEntryVisibleState )
		{			
			ConsoleWindow.Trace("Finished hiding previous page, showing current page: " + mCurrentPage);
			TogglePageVisibility( mCurrentPage, true );		
		}	
		else
		{
			mcPageNavigator.Enable();
		}
	}
	
	//override this in extended class to show/hide specific movieclips within mcEntry
	private function OnShowMovieClipEntry( mcEntry:MovieClip, index:Number ):Void 
	{
		mcEntry.mcLine._visible = ( index != 0 );
	}
	
	private function OnHideMovieClipEntry( mcEntry:MovieClip ):Void 
	{
	}
	
	private function OnFinishedHidingEntry( mcEntry:MovieClip ):Void
	{
		
	}
	
	private function RemoveEntryFromList( _entry:MovieClip, _list:Array, bInstantRefresh:Boolean ):Void
	{
		var index:Number = -1;
		var length = mMovieClipList.length;
		for ( var i:Number = 0; i < length; i++ )
		{
			var entry:MovieClip = mMovieClipList[i];
			//ConsoleWindow.Trace("entry = " + entry);
			if ( entry == _entry )
			{
				index = i;
				break;
			}
		}
		
		if ( index >= 0 )
		{
			mMovieClipList.splice( index, 1 );
			_entry.removeMovieClip();	
			
			if ( bInstantRefresh )
			{
				InstantShowPage();			
			}
			else
			{
				mFirstEntryIndex = mCurrentPage * mEntriesPerPage - ( mEntriesPerPage );
				mCurEntryIndex = index - mFirstEntryIndex;
				ConsoleWindow.Trace("Entry removed.  mFirstEntryIndex = " + mFirstEntryIndex + " mCurEntryIndex = " + mCurEntryIndex);
				for ( var n:Number = mCurEntryIndex; n < mEntriesPerPage; n++ )
				{				
					var mcEntry:MovieClip = mMovieClipList[mFirstEntryIndex + n];
					
					if ( n == mEntriesPerPage - 1 )
					{
						mcEntry._visible = true;
						mcEntry._x = mFirstEntryX;
						mcEntry._y = mFirstEntryY + (n+1) * (mcEntry._height + mDeltaY);
						new Tween( mcEntry, "_alpha", None.easeNone, 0, 100, .3, true );
					}
					
					var targetY:Number = mFirstEntryY + (n) * (mcEntry._height + mDeltaY);	
					new Tween( mcEntry, "_y", None.easeNone, mcEntry._y, targetY, .3, true );
				}		
			}
			OnMoveClipListUpdated();
		}
		else
		{
			ConsoleWindow.Trace("WARNING! RemoveEntryFromList(), can't find the right entry to remove!  entry = " + _entry);
		}		
				
		mFirstEntryIndex = mCurrentPage * mEntriesPerPage - ( mEntriesPerPage );
		//User deleted all the entries on the current page, manually show previous page
		if ( mFirstEntryIndex >= mMovieClipList.length && mCurrentPage > 1 )
		{
			mcPageNavigator.PreviousPage();
		}
		//ConsoleWindow.Trace( " mFirstEntryIndex = " + mFirstEntryIndex + " mMovieClipList.length = " + mMovieClipList.length);
	}
}