

import mx.utils.Delegate;

class PageNavigator extends MovieClip
{	
	[Inspectable(name = "Page Total", type = "Number", defaultValue = 1)]
	private var mTotalPages:Number;
	
	private var mcLeftArrow:MovieClip;
	private var mcRightArrow:MovieClip;
	private var tfPage:TextField;
	private var mCurPage:Number;
	
	private var mNextPageCallback:Function;
	private var mPreviousPageCallback:Function;
	
	private var mIsEnabled:Boolean;
	
	public function PageNavigator()
	{
		mCurPage = 1;
		mTotalPages = 1;
		mIsEnabled = true;
	}	
	
	public function onLoad():Void
	{
		OnPageChanged();
		
		mcRightArrow.onPress = Delegate.create( this, NextPage );
		mcLeftArrow.onPress = Delegate.create( this, PreviousPage );		
	}
	
	public function AddCallbacks( _onNextPage:Function, _onPreviousPage:Function )
	{
		mNextPageCallback = _onNextPage;
		mPreviousPageCallback = _onPreviousPage;
		
		ConsoleWindow.Trace( "Adding calls: " + _onNextPage + ", " + _onPreviousPage );
	}
	
	public function Disable():Void
	{
		mIsEnabled = false;
	}
	
	public function Enable():Void
	{
		mIsEnabled = true;
	}
	
	public function SetTotalPages( _total ):Void
	{
		mTotalPages = _total;
		OnPageChanged();
		_visible = ( mTotalPages > 0 );
		
	}
	
	public function GoToPage( _page:Number ):Void
	{
		if ( _page > 0 && _page <= mTotalPages )
		{
			mCurPage = 1;
			OnPageChanged();
		}
	}
	
	private function NextPage():Void
	{
		if ( mIsEnabled && mCurPage + 1 <= mTotalPages )
		{
			mCurPage++;
			OnPageChanged();
			mNextPageCallback.call();
		}
	}
	
	public function PreviousPage():Void
	{
		if ( mIsEnabled && mCurPage - 1 > 0 )
		{
			mCurPage--;
			OnPageChanged();
			mPreviousPageCallback.call();
		}
	}
	
	private function OnPageChanged():Void
	{
		tfPage.text = String(mCurPage) + " OF " + String(mTotalPages);
		mcLeftArrow._visible = true;
		mcRightArrow._visible = true;
		if ( mCurPage == mTotalPages )
		{
			mcRightArrow._visible = false;
		}
		if ( mCurPage == 1 )
		{
			mcLeftArrow._visible = false;
		}		
	}
	
}