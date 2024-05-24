function v2(useWorkArea, nPts, nIters) {

	var comp = app.project.activeItem;

	// run every frame
	var df = comp.frameDuration;
	var fStart = comp.workAreaStart;
	var fEnd = fStart + comp.workAreaDuration;

	var textLayer = comp.layer("text");
	var srcLayer = comp.layer("src");

	if(comp.layer("paper")) {
		comp.layer("paper").remove();
	}

	// enable the first time
	textLayer.property("Source Text").expressionEnabled = true;

	// hack: sampleImage is only available on layer expressions
	// so we will run sampleImage and report values to text layer's src value
	// then read the src value and report it back to script
	function getA(x, y, r) 
	{
		var expr = "thisComp.layer('src').sampleImage([" + x + "," + y + "], [" + r + "," + r +"])[3];"
		textLayer.property("Source Text").expression = expr;
		return textLayer.text.sourceText.value;
	}

	var w = srcLayer.width;
	var h = srcLayer.height;

	var xmin = - srcLayer.width / 2;
	var ymin = - srcLayer.height / 2;
	var xmax = srcLayer.width / 2;
	var ymax = srcLayer.height / 2;

	var xc = (xmax + xmin) / 2;
	var yc = (ymax + ymin) / 2;
	var c = [xc, yc];

	var xDst = (xmax - xmin);
	var yDst = (ymax - ymin);
	var dst = 2 * xDst + 2 * yDst;

	// parameterize corners in terms of time t in range [0,1]
	var t1 = xDst / dst;
	var t2 = 0.5;
	var t3 = t1 + t2;
	var t4 = 1.0;

	function dx(t) { 
		if(t < t1) { return xmin + (t / t1) * xDst; }
		if(t < t2) { return xmin + xDst; }
		if(t < t3) { return xmin + (1 - ((t - t2) / (t3 - t2))) * xDst; }
		return xmin;
	}

	function dy(t) { 
		if(t < t1) { return ymin; }
		if(t < t2) { return ymin + ((t - t1) / (t2 - t1)) * yDst; }
		if(t < t3) { return ymin + yDst; }
		return ymin + (1 - ((t - t3) / (t4 - t3))) * yDst;
	}

	var shapeLayer = comp.layers.addShape();
	var shapeGroup = shapeLayer.property("ADBE Root Vectors Group"); 
	shapeGroup.addProperty("ADBE Vector Shape - Group"); 
	var fill = shapeGroup.addProperty("ADBE Vector Graphic - Fill");
	fill.property("Color").setValue([255,255,255]);
	shapeLayer.name = "paper";
	shapeLayer.moveToEnd();

	// parameterize walking on bbox by time t
	// to create uniform initial point distribution 
	var pts = []
	for(var i = 0; i < nPts; i++) {
		var t = i / nPts;
		// make sure to center pts
		var x = dx(t);
		var y = dy(t);
		pts.push([x,y]);
	}

	var r = 40
	comp.time = fStart;
	var fNum = 1;
	while(comp.time < fEnd) {

		for(var j = 0; j < nPts; j++) {
			var p = pts[j];
			var v = c - p;
			var pc = p + [w/2, h/2];

			// if point lives inside src, move back out
			while(getA(pc[0], pc[1], r) > 0.01) {
				pc -= 0.2 * v;
			}

			pts[j] = pc - [w/2, h/2];
		}

		
		for(var j = 0; j < nPts; j++) {
			var dt = 0.25;
			for(var i = 0; i < nIters; i++) {
				var p = pts[j];
				var v = c - p;
				p = p + dt * v;

				// image sampling done in [0,0] -> [w,h] space
				// points are in [-w/2,-h/2] -> [w/2,h/2] space
				// need to convert for check
				var pc = p + [w/2, h/2];
				if(getA(pc[0], pc[1], r) < 0.01) {
					pts[j] = p;
				}
				else {
					dt /= 2.0;
				}

				if(dt < 0.05) {
					break;
				}
			}
		}

		var shape = new Shape();
		shape.vertices = pts;
		shape.closed = true;
		shapeGroup.property(1).property("ADBE Vector Shape").setValueAtTime(comp.time, shape);
		shapeGroup.property(1).property("ADBE Vector Shape").setInterpolationTypeAtKey(fNum, KeyframeInterpolationType.HOLD);

		comp.time += df;
		fNum++;
	}
}


