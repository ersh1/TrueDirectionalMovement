import com.greensock.TimelineLite;
import com.greensock.easing.*;

class TargetLock.TargetBar extends MovieClip
{
	public var HealthBar: MovieClip;
	public var PhantomBar: MovieClip;
	public var PhantomMask: MovieClip;
	public var BarBackground: MovieClip;
	public var Damage: TextField;
	public var LevelFrame: MovieClip;
	public var LevelIcon: MovieClip;
	public var LevelText: TextField;
	public var LevelMask: MovieClip;
	public var FrameOutline: MovieClip;
	
	var showHideTimeline: TimelineLite;
	var phantomTimeline: TimelineLite;
	var phantomAlphaTimeline: TimelineLite;
	
	var phantomDuration: Number;
	var desiredBarAlpha: Number;

	public function TargetBar() 
	{
		// constructor code
		this.stop();
		
		showHideTimeline = new TimelineLite({paused:true});
		phantomTimeline = new TimelineLite({paused:true});
		phantomAlphaTimeline = new TimelineLite({paused:true});
		Damage._visible = false;
		desiredBarAlpha = 0;
		_alpha = 0;
	}
	
	public function showBar() : Void
	{
		showHideTimeline.clear();
		
		showHideTimeline.set(this, {_alpha:0}, 0);
		showHideTimeline.to(this, 0.25, {_alpha:desiredBarAlpha}, 0);
		showHideTimeline.restart();
	}
	
	public function setLevelIconColor(ColorNumber: Number) : Void
	{
		var NewColor: Color = new Color(LevelIcon);
		NewColor.setRGB(ColorNumber);
	}
	
	public function setLevelTextColor(ColorNumber: Number) : Void
	{
		LevelText.textColor = ColorNumber;
	}
	
	public function setFrameColor(ColorNumber: Number) : Void
	{
		var NewColor: Color = new Color(FrameOutline);
		NewColor.setRGB(ColorNumber);
	}

	public function setHealthPercent(CurrentPercent: Number) : Void
	{
		phantomTimeline.clear()
			
		var newWidth = BarBackground._width * CurrentPercent;
		
		HealthBar.Mask._width = newWidth;
		PhantomBar.Bar.Mask._width = newWidth;
		PhantomBar.Mask._width = BarBackground._width - newWidth;
	}
	
	public function updateHealthPercent(PreviousPercent: Number, CurrentPercent: Number) : Void
	{
		//var previousWidth = BarBackground._width * PreviousPercent;
		var newWidth = BarBackground._width * CurrentPercent;
		
		HealthBar.Mask._width = newWidth;
		PhantomBar.Mask._width = BarBackground._width - newWidth;

		updatePhantom(newWidth);
	}
	
	public function updateDamage(NewDamage: Number) : Void
	{
		Damage.text = NewDamage.toString();
		Damage._visible = true;
	}
	
	public function hideDamage() : Void
	{
		Damage._visible = false;
	}
	
	public function updatePhantom(NewWidth: Number) : Void
	{
		if (!phantomTimeline.isActive())
		{
			phantomTimeline.clear();
			phantomTimeline.progress(0);
			phantomTimeline.restart();
			PhantomBar._alpha = 100;
		}
				
		phantomTimeline.to(PhantomBar.Bar.Mask, 0.25, {_width: NewWidth}, phantomTimeline.time() + phantomDuration);
		phantomTimeline.play();
		
		phantomAlphaTimeline.clear();
		phantomAlphaTimeline.to(PhantomBar, 0, {_alpha:0}, phantomDuration + 0.25);
		phantomAlphaTimeline.restart();
	}
	
	public function phantomBarVisibility(Visible: Boolean) : Void
	{
		PhantomBar._visible = Visible;
	}
	
	public function levelDisplayNone() : Void
	{
		LevelFrame._visible = false;
		LevelIcon._visible = false;
		LevelText._visible = false;
		LevelMask._visible = false;
	}
	
	public function levelDisplayIcon() : Void
	{
		LevelFrame._visible = true;
		LevelIcon._visible = true;
		LevelText._visible = false;
		LevelMask._visible = true;
	}
	
	public function levelDisplayText() : Void
	{
		LevelFrame._visible = true;
		LevelIcon._visible = false;
		LevelText._visible = true;
		LevelMask._visible = true;
	}
	
	public function levelDisplayOutline() : Void
	{
		LevelFrame._visible = false;
		LevelIcon._visible = false;
		LevelText._visible = false;
		LevelMask._visible = false;
	}
	
	public function setPhantomDuration(Duration: Number) : Void
	{
		phantomDuration = Duration;
	}
	
	public function setBarAlpha(Alpha: Number) : Void
	{
		desiredBarAlpha = Alpha;
		//_alpha = Alpha;
	}
}
