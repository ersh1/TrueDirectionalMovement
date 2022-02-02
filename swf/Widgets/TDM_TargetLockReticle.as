import com.greensock.TimelineLite;
import com.greensock.TimelineMax;
import com.greensock.easing.*;

class Widgets.TDM_TargetLockReticle extends MovieClip
{
	public var ReticleOuter: MovieClip;

	var idleTimeline: TimelineMax;

	var showHideTimeline: TimelineLite;
	var moveTimeline: TimelineLite;

	var reticleType: Number;
	var reticleAlpha: Number;

	var circle_mc: MovieClip;
	var circleRadius: Number;
	var circleAlpha: Number;

	var bPendingRemoval = false;
	var bInitialized = false;

	var bReadyToRemove = false;

	public function TDM_TargetLockReticle() 
	{
		// constructor code
		this.stop();
		
		idleTimeline = new TimelineMax({paused:true, repeat:-1, yoyo:true});
		showHideTimeline = new TimelineLite({paused:true});
		moveTimeline = new TimelineLite({paused:true, onComplete:function() { this.reverse() }});

		circle_mc = ReticleOuter.Reticle.createEmptyMovieClip("circle", this.getNextHighestDepth());
		circle_mc.circleRadius = 100;
		circle_mc.circleAlpha = 0;

		// delay init till everything's ready
		this.onEnterFrame = function(): Void {
			init();
			delete this.onEnterFrame;
		}
	}

	public function init()
	{
		idleTimeline.clear();
		idleTimeline.fromTo(ReticleOuter, 0.5, {_xscale:100, _yscale:100}, {_xscale:110, _yscale:110}, 0);

		ReticleOuter._alpha = reticleAlpha;

		switch(reticleType) 
		{
		case 0: // Crosshair
			ReticleOuter.Reticle.ReticleTop._visible = true;
			ReticleOuter.Reticle.ReticleBottom._visible = true;
			ReticleOuter.Reticle.ReticleLeft._visible = true;
			ReticleOuter.Reticle.ReticleRight._visible = true;
			ReticleOuter.Reticle.ReticleSimpleGlow._visible = false;

			showHideTimeline.clear();
			showHideTimeline.eventCallback("onReverseComplete", setReadyToRemove, [true], this);
			showHideTimeline.fromTo(ReticleOuter.Reticle, 0.5, {_rotation:0}, {_rotation:-45, ease:Linear.easeNone}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleTop, 0.5, {_alpha:75}, {_alpha:100}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleBottom, 0.5, {_alpha:75}, {_alpha:100}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleLeft, 0.5, {_alpha:75}, {_alpha:100}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleRight, 0.5, {_alpha:75}, {_alpha:100}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleTop, 0.5, {_rotation:0, _y:-8.5}, {_rotation:-180, _y:-15, ease:Sine.easeIn}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleBottom, 0.5, {_rotation:180, _y:8.5}, {_rotation:0, _y:15, ease:Sine.easeIn}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleLeft, 0.5, {_rotation:-90, _x:-8.5}, {_rotation:-270, _x:-15, ease:Sine.easeIn}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleRight, 0.5, {_rotation:90, _x:8.5}, {_rotation:-90, _x:15, ease:Sine.easeIn}, 0);

			moveTimeline.clear();
			moveTimeline.fromTo(ReticleOuter.Reticle.ReticleTop, 0.25, {_y:-15}, {_y:-30}, 0);
			moveTimeline.fromTo(ReticleOuter.Reticle.ReticleBottom, 0.25, {_y:15}, {_y:30}, 0);
			moveTimeline.fromTo(ReticleOuter.Reticle.ReticleLeft, 0.25, {_x:-15}, {_x:-30}, 0);
			moveTimeline.fromTo(ReticleOuter.Reticle.ReticleRight, 0.25, {_x:15}, {_x:30}, 0);

			break;
		case 1: // CrosshairNoTransform
			ReticleOuter.Reticle.ReticleTop._visible = true;
			ReticleOuter.Reticle.ReticleBottom._visible = true;
			ReticleOuter.Reticle.ReticleLeft._visible = true;
			ReticleOuter.Reticle.ReticleRight._visible = true;
			ReticleOuter.Reticle.ReticleSimpleGlow._visible = false;

			showHideTimeline.clear();
			showHideTimeline.eventCallback("onReverseComplete", setReadyToRemove, [true], this);
			showHideTimeline.set(ReticleOuter.Reticle.ReticleTop, {_rotation:-180}, 0);
			showHideTimeline.set(ReticleOuter.Reticle.ReticleBottom, {_rotation:0}, 0);
			showHideTimeline.set(ReticleOuter.Reticle.ReticleLeft, {_rotation:-270}, 0);
			showHideTimeline.set(ReticleOuter.Reticle.ReticleRight, {_rotation:-90}, 0);
			showHideTimeline.set(ReticleOuter.Reticle, {_rotation:-45}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleTop, 0.5, {_alpha:0, _y:-30}, {_alpha:100, _y:-15, ease:Sine.easeInOut}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleBottom, 0.5, {_alpha:0, _y:30}, {_alpha:100, _y:15, ease:Sine.easeInOut}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleLeft, 0.5, {_alpha:0, _x:-30}, {_alpha:100, _x:-15, ease:Sine.easeInOut}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleRight, 0.5, {_alpha:0, _x:30}, {_alpha:100, _x:15, ease:Sine.easeInOut}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleSimpleGlow, 0.5, {_alpha:0}, {_alpha:100, ease:Sine.easeInOut}, 0);

			moveTimeline.clear();
			moveTimeline.fromTo(ReticleOuter.Reticle.ReticleTop, 0.25, {_y:-15}, {_y:-30}, 0);
			moveTimeline.fromTo(ReticleOuter.Reticle.ReticleBottom, 0.25, {_y:15}, {_y:30}, 0);
			moveTimeline.fromTo(ReticleOuter.Reticle.ReticleLeft, 0.25, {_x:-15}, {_x:-30}, 0);
			moveTimeline.fromTo(ReticleOuter.Reticle.ReticleRight, 0.25, {_x:15}, {_x:30}, 0);

			break;
		case 2: // Dot
			ReticleOuter.Reticle.ReticleTop._visible = false;
			ReticleOuter.Reticle.ReticleBottom._visible = false;
			ReticleOuter.Reticle.ReticleLeft._visible = false;
			ReticleOuter.Reticle.ReticleRight._visible = false;
			ReticleOuter.Reticle.ReticleSimpleGlow._visible = false;

			showHideTimeline.clear();
			showHideTimeline.eventCallback("onUpdate", drawCircles, null, this);
			showHideTimeline.eventCallback("onComplete", drawDot, null, this);
			showHideTimeline.eventCallback("onReverseComplete", setReadyToRemove, [true], this);
			showHideTimeline.fromTo(circle_mc, 0.5, {circleRadius:40}, {circleRadius:4, ease:Sine.easeIn}, 0);
			showHideTimeline.fromTo(circle_mc, 0.5, {circleAlpha:0}, {circleAlpha:80, ease:Sine.easeIn}, 0);

			moveTimeline.clear();
			moveTimeline.eventCallback("onUpdate", drawCircles, null, this);
			moveTimeline.eventCallback("onReverseComplete", drawDot, null, this);
			moveTimeline.to(circle_mc, 0.25, {circleAlpha:80, ease:Sine.easeIn}, 0);
			moveTimeline.fromTo(circle_mc, 0.25, {circleRadius:4}, {circleRadius:20, ease:Sine.easeOut}, 0);

			break;
		case 3: // Glow
			ReticleOuter.Reticle.ReticleTop._visible = false;
			ReticleOuter.Reticle.ReticleBottom._visible = false;
			ReticleOuter.Reticle.ReticleLeft._visible = false;
			ReticleOuter.Reticle.ReticleRight._visible = false;
			ReticleOuter.Reticle.ReticleSimpleGlow._visible = true;

			showHideTimeline.clear();
			showHideTimeline.eventCallback("onReverseComplete", setReadyToRemove, [true], this);
			showHideTimeline.set(ReticleOuter.Reticle.ReticleTop, {_visible:false}, 0);
			showHideTimeline.set(ReticleOuter.Reticle.ReticleBottom, {_visible:false}, 0);
			showHideTimeline.set(ReticleOuter.Reticle.ReticleLeft, {_visible:false}, 0);
			showHideTimeline.set(ReticleOuter.Reticle.ReticleRight, {_visible:false}, 0);
			showHideTimeline.fromTo(ReticleOuter.Reticle.ReticleSimpleGlow, 0.5, {_alpha:0, _xscale:300, _yscale:300}, {_alpha:100, _xscale:100, _yscale:100, ease:Sine.easeInOut}, 0);

			moveTimeline.clear();
			moveTimeline.fromTo(ReticleOuter.Reticle, 0.25, {_xscale:100, _yscale:100}, {_xscale:150, _yscale:150, ease:Sine.easeInOut}, 0);

			break;
		}	

		bInitialized = true;

		playInitTimeline();
		
	}

	public function cleanUp()
	{
		idleTimeline.clear();
		idleTimeline.kill();
		idleTimeline = null;

		showHideTimeline.eventCallback("onUpdate", null);
		showHideTimeline.eventCallback("onComplete", null);
		showHideTimeline.eventCallback("onReverseComplete", null);
		showHideTimeline.clear();
		showHideTimeline.kill();
		showHideTimeline = null;	

		moveTimeline.eventCallback("onUpdate", null);
		moveTimeline.eventCallback("onReverseComplete", null);
		moveTimeline.clear();
		moveTimeline.kill();
		moveTimeline = null;
	}

	public function drawCircles()
	{
		var radius = circle_mc.circleRadius;

		circle_mc.clear();

		// draw outer black circle
		circle_mc.lineStyle(1, 0x000000, circle_mc.circleAlpha / 2);
		drawCircle(radius);

		// draw inner white circle
		circle_mc.lineStyle(2, 0xCBCBCB, circle_mc.circleAlpha);
		drawCircle(radius-2);
	}

	public function drawDot()
	{
		var radius = circle_mc.circleRadius;

		// circle_mc.clear();
		// circle_mc.lineStyle(1, 0x000000, circle_mc.circleAlpha / 2);
		// circle_mc.beginFill(0xCBCBCB, circle_mc.circleAlpha);
		// drawCircle(radius);
		// circle_mc.endFill();

		circle_mc.clear();
		circle_mc.lineStyle(1.5, 0x000000, circle_mc.circleAlpha / 2);
		drawCircle(radius);

		circle_mc.lineStyle(0, 0x000000, 0);
		circle_mc.beginFill(0xCBCBCB, circle_mc.circleAlpha);
		drawCircle(radius - 0.75);
		circle_mc.endFill();
	}

	// https://www.oreilly.com/library/view/actionscript-cookbook/0596004907/ch04s06.html
	public function drawCircle(radius: Number) 
	{
		// The angle of each of the eight segments is 45 degrees (360 divided by 8), which
		// equals π/4 radians.
		var angleDelta = Math.PI / 4;

		// Find the distance from the circle's center to the control points for the curves.
		var ctrlDist = radius/Math.cos(angleDelta/2);

		// Initialize the angle to 0 and define local variables that are used for the 
		// control and ending points. 
		var angle = 0;
		var rx, ry, ax, ay;

		// Move to the starting point, one radius to the right of the circle's center.
		circle_mc.moveTo(radius, 0);

		// Repeat eight times to create eight segments.
		for (var i = 0; i < 8; i++)
		{
			// Increment the angle by angleDelta (π/4) to create the whole circle (2π).
			angle += angleDelta;

			// The control points are derived using sine and cosine.
			rx = Math.cos(angle-(angleDelta/2))*(ctrlDist);
			ry = Math.sin(angle-(angleDelta/2))*(ctrlDist);

			// The anchor points (end points of the curve) can be found similarly to the 
			// control points.
			ax = Math.cos(angle)*radius;
			ay = Math.sin(angle)*radius;

			// Draw the segment.
			circle_mc.curveTo(rx, ry, ax, ay);
		}
	}

	public function playIdleTimeline()
	{
		idleTimeline.play();
	}

	public function playChangeTargetTimeline()
	{
		if (reticleType == 2) // Dot
		{
			showHideTimeline.pause();
		}

		if (moveTimeline.progress() != 1)
		{
			moveTimeline.play();
		}
		else
		{
			moveTimeline.reverse();
		}
	}
	
	public function playInitTimeline()
	{
		moveTimeline.pause();

		if (showHideTimeline.isActive())
		{
			showHideTimeline.play();
		}
		else
		{
			showHideTimeline.restart();
		}

		playIdleTimeline();
	}

	public function playRemovalTimeline()
	{
		idleTimeline.pause();

		if (reticleType == 2) // Dot or Glow
		{
			moveTimeline.pause();
		}
		else
		{
			moveTimeline.reverse();
		}

		showHideTimeline.reverse();		
		if (showHideTimeline.progress() == 0)
		{
			setReadyToRemove(true);
		}
	}

	public function updateData(a_bPendingRemoval: Boolean)
	{
		if (!bInitialized)
		{
			return;
		}

		if (a_bPendingRemoval != bPendingRemoval)
		{
			bPendingRemoval = a_bPendingRemoval;
			if (bPendingRemoval) {
				playRemovalTimeline();
			} else {
				playInitTimeline();
			}
		}
	}
	
	public function loadConfig(a_reticleType: Number, a_reticleAlpha: Number)
	{
		reticleType = a_reticleType;
		reticleAlpha = a_reticleAlpha;
	}

	public function setReadyToRemove(a_readyToRemove: Boolean)
	{
		bReadyToRemove = a_readyToRemove;
	}

	public function isReadyToRemove() : Boolean
	{
		return bReadyToRemove;
	}
}
