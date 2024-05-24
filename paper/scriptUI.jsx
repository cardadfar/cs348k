#include v1.jsx
#include v2.jsx
#include v3.jsx
#include v4.jsx

(function (thisObj) {
    expressionPanel(thisObj);
    var applyBtn;
    var applyCustomBtn;
    var expressionInput;
    var expressionDropdown;
    function expressionPanel(thisObj) {
        var win = (thisObj instanceof Panel) ? thisObj : new Window('palette', 'Paper Script', undefined);
        win.spacing = 6;
        var tabPanel = win.add('tabbedpanel');
        tabPanel.alignChildren = ['fill', 'fill'];
        tabPanel.preferredSize = [350, 300];
        // -- Tab One --
        var tabOne = tabPanel.add('tab', undefined, 'Parameters');
        tabOne.alignChildren = 'right';
        
        // Expression Dropdown

        var applyGroup = tabOne.add('group', undefined, 'applyGroup');

        applyGroup.alignChildren = 'right';
        var dropdownGroup = applyGroup.add('group', undefined, 'dropdownGroup');
        //dropdownGroup.orientation = 'row';
        dropdownGroup.add ("statictext", undefined, "Version: ");
        expressionDropdown = dropdownGroup.add('dropdownlist', undefined, [
            "v4",
            "v3",
            "v2",
            "v1"
        ]);
        expressionDropdown.selection = 0;
        expressionDropdown.preferredSize.width = 240;

        // Apply Button
        var btnGroup = applyGroup.add('group', undefined, 'btnGroup');
        //btnGroup.orientation = 'row';
        applyBtn = btnGroup.add('button', undefined, 'Apply');

        tabOne.alignChildren = 'left';
        var useWorkAreaBox = tabOne.add('checkbox', undefined, "use Work Area");
        useWorkAreaBox.value = true;

        var numPointsSliderGroup = tabOne.add('group', undefined, 'numPointsSliderGroup');
        //numPointsSliderGroup.orientation = "row";
		numPointsSliderGroup.add("statictext", undefined, "points");
		var numPointsSliderValue = numPointsSliderGroup.add("slider", undefined, "");
		numPointsSliderValue.minvalue = 1;
		numPointsSliderValue.maxvalue = 100;
		numPointsSliderValue.value = 30;
		var numPointsSliderText = numPointsSliderGroup.add("statictext", undefined, "30");
		numPointsSliderText.characters = 4;

		numPointsSliderValue.onChanging = function() {
		        numPointsSliderText.text = Math.floor(numPointsSliderValue.value);
		    }

        var numItersSliderGroup = tabOne.add('group', undefined, 'numItersSliderGroup');
        //numItersSliderGroup.orientation = "row";
		numItersSliderGroup.add("statictext", undefined, "iterations");
		var numItersSliderValue = numItersSliderGroup.add("slider", undefined, "");
		numItersSliderValue.minvalue = 5;
		numItersSliderValue.maxvalue = 100;
		numItersSliderValue.value = 10;
		var numItersSliderText = numItersSliderGroup.add("statictext", undefined, "10");
		numItersSliderText.characters = 4;

		numItersSliderValue.onChanging = function() {
		        numItersSliderText.text = Math.floor(numItersSliderValue.value);
		    }

        // -- Tab Two --
        var tabTwo = tabPanel.add('tab', undefined, 'Custom Expression');
        tabTwo.alignChildren = 'fill';
        // Custom Expression Input
        expressionInput = tabTwo.add('edittext', undefined, 'loopOut();');
        expressionInput.characters = 25;
        // Apply Custom Button
        applyCustomBtn = tabTwo.add('button', undefined, 'Apply');
        // --- ACTIONS ---
        applyCustomBtn.onClick = function () {
            applyCustomExpression(expressionInput.text);
        };
        applyBtn.onClick = function () {
        	var fnType = expressionDropdown.selection.text;
        	var nPts = Math.floor(numPointsSliderValue.value);
        	var nIters = Math.floor(numItersSliderValue.value);
        	var useWorkArea = useWorkAreaBox.value;

            var startTime = new Date();

            if(fnType == "v4") {
                v4(useWorkArea, nPts, nIters);
            }
            else if(fnType == "v3") {
            	v3(useWorkArea, nPts, nIters);
            }
            else if (fnType == "v2") {
            	v2(useWorkArea, nPts, nIters);
            }
            else if (fnType == "v1") {
                v1(useWorkArea, nPts, nIters);
            }

            var endTime = new Date();
            var timeElapsed = endTime.getTime() - startTime.getTime();
            alert(timeElapsed);

        };
        win.onResizing = win.onResize = function () {
            this.layout.resize();
        };
        win instanceof Window
            ? (win.center(), win.show()) : (win.layout.layout(true), win.layout.resize());
    }
    function applyCustomExpression(expression) {
        app.beginUndoGroup('My Script Action');
        var selected = app.project.activeItem.selectedProperties;
        if (selected.length == 0) {
            alert('Please select a property');
        } else {
            for (var i = 0; i < selected.length; i++) {
                selected[i].expression = expression;
            }
        }
        app.endUndoGroup();
    }
})(this);