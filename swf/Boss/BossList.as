import com.greensock.TimelineLite;

class Boss.BossList extends MovieClip
{

	public function BossBar() 
	{
		// constructor code
		super();
		
		this.stop();
	}
	
	public function playTimeline() : Void
	{
		var tl = new TimelineLite({paused:true});
		
		tl.set(this, {_alpha:0}, 0);
		tl.to(this, 0.25, {_alpha:100}, 0);
		tl.play();
	}
}
