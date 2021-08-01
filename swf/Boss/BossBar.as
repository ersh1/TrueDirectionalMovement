import com.greensock.TimelineLite;
import com.greensock.easing.*;

class Boss.BossBar extends MovieClip
{
	public var HealthBar: MovieClip;
	public var PhantomBar: MovieClip;
	public var PhantomMask: MovieClip;
	public var BarBackground: MovieClip;
	public var Damage: TextField;
	public var LevelFrame: MovieClip;
	public var LevelIcon: MovieClip;
	public var LevelText: TextField;
	public var LeftOrnament: MovieClip;
	public var TargetName: TextField;
		
	var showHideTimeline: TimelineLite;
	var phantomTimeline: TimelineLite;
	var phantomAlphaTimeline: TimelineLite;
	
	var phantomDuration: Number;
	var desiredBarAlpha: Number;
	
	public function BossBar() 
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
	
	public function showBoss() : Void
	{
		showHideTimeline.clear();
		
		showHideTimeline.set(this, {_alpha:0}, 0);
		showHideTimeline.to(this, 0.5, {_alpha:desiredBarAlpha}, 0);
		showHideTimeline.restart();
	}
	
	public function removeBoss() : Void
	{
		showHideTimeline.clear();
		
		showHideTimeline.set(this, {_alpha:desiredBarAlpha}, 0);
		showHideTimeline.to(this, 1.0, {_alpha:0}, 3.0);
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

	public function setHealthPercent(CurrentPercent: Number) : Void
	{
		phantomTimeline.clear();
				
		var newWidth = BarBackground._width * CurrentPercent;
		
		HealthBar.Mask._width = newWidth;
		PhantomBar.Mask._width = newWidth;
		PhantomMask._width = BarBackground._width - newWidth;
	}
	
	public function updateHealthPercent(PreviousPercent: Number, CurrentPercent: Number) : Void
	{
		//var previousWidth = BarBackground._width * PreviousPercent;
		var newWidth = BarBackground._width * CurrentPercent;
		
		HealthBar.Mask._width = newWidth;
		PhantomMask._width = BarBackground._width - newWidth;
		
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
				
		phantomTimeline.to(PhantomBar.Mask, 0.25, {_width: NewWidth}, phantomTimeline.time() + phantomDuration);
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
		LeftOrnament._visible = true;
	}
	
	public function levelDisplayIcon() : Void
	{
		LevelFrame._visible = true;
		LevelIcon._visible = true;
		LevelText._visible = false;
		LeftOrnament._visible = false;
	}
	
	public function levelDisplayText() : Void
	{
		LevelFrame._visible = true;
		LevelIcon._visible = false;
		LevelText._visible = true;
		LeftOrnament._visible = false;
	}
	
	public function bossNameAlignment(Align: String) : Void
	{
		TargetName.autoSize = Align;
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
