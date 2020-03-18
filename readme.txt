This is my capstone project as a senior at DigiPen Institute of Technology in the Bachelor of Science in Computer Science and Digital Audio program. 
The goal of the project is to use a genetic algorithm to optimize random MIDI notes into enjoyable melodies

To compile the project use the provided visual studio solution. It's built to be an x64 program and may not compile when building in x86, 
I've testing building under visual studio 2017 and 2019. In visual studio 2019 you will need to retarget the project/solution.
If you find any errors while building let me know and I'll take a look at it. 

How to use the program:

Some UI buttons are just placeholders for the time being, such as the drop downs for phrase length, and smallest subdivision

To see the generated phrases click on the buttons along the bottom left side of the screen, the currently highlighted one is
displayed on the piano roll to the right. 

The piano roll can be scrolled up and down on using the mouse wheel as well as by dragging the slider bar on the right. Side-to-side
scrolling can be accomplished by holding shift while using the mouse wheel or dragging the slider bar along the bottom.

Once a phrase has been selected it can be played back using the Play button on the left center panel, and stopped with the Stop button.

To access the rules used in determining musical quality, click View->Rule Builder along the top of the window. 
Rules can be created using the "Add Rule" Button on the top left, or imported from a file using the import button.

Currently, rules can only analyze pitch values, other rule types will have no effect on the output of the algorithm. The evaluation method is
provided by the functions attached to each rule. There are 4 buttons on the right side of the Rule Builder window to create, copy, rename, and delete
functions. The function named "Default Function" cannot be renamed, deleted, or edited in any way.

Below is a graph used to edit and display the function attached to the current rule. The graph can be moved by right clicking and dragging the mouse, 
and vertices can be moved by left clicking and dragging. To add vertices right click and empty space on the graph and click add vertex in the context
box that appears, vertices can be deleted in a similar manner.

Currently the project comes with one preset. This preset is named "PitchSet" and can imported using the import button at the top left of the Rule Builder window.
NOTE: Importing a file will delete all existing rules from the Rule Builder

Once a set of rules has been created, it can be saved by clicking the export button and providing a name for the preset.