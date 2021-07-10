$fn=100;


pcb_tol = 0.25;    // Tolerance spacing on each side of PCB
pcb_width=32;      // Width of PCB
pcb_length=66;     // Length of PCB

fit_delta=0.15;         // Tolerance for interference fits
top_depth = 4;          // Top overall depth
top_top_thickness = 2;  // Top plate thickness
pcb_thickness = 1.6;    // PCB Thickness

border=8;                                // Excess border beyond PCB limits left & right
outer_width=pcb_width+border+pcb_tol*2;  // Outer size of case
outer_length=pcb_length+2+pcb_tol*2;     // Outer length of case
base_depth=7;                            // Overall depth of case base
base_top_thickness=3;                    // Bottom plate thickness

// Top recess for label
tr_ofs=1.5;                             // Label lip offset
tr_depth=0.25;                          // Label lip depth

// Front lip for USB protection
lip_extension = 3;                      // How far below the bottom of the case the 'chin' comes

micro=0.01;                             // Tolerancing offset for cuts

pcb_depth=8.5;
pcb_tray_depth=5.2;
pcb_hole_displacement=3;
pcb_support = 5.5;
pcb_support_z_radius = 1;
z_radius=3;

usb_width=10;
usb_below=3.2;

bulb_length = 33;
bulb_width = pcb_width + 9*2;
bulb_depth=base_depth;

side_pins_height = 3.2;
side_pins_width  = 22;
side_pins_length = bulb_width+micro;
side_pins_north_ofs = 7;

screw_radius = 1.5;
screw_head_radius = 2.5;
screw_head_depth = 1.55;
screw_hole_depth = bulb_depth+2*micro;
screw_countersink_depth	= 0;

// Holes
top_hole_depth = 4.5;
top_clearance = 3;
top_tap_holesize = 1.15;
top_tap_holedepth=top_hole_depth;
top_tap_offs = 1.0;

// Debug connectors
dbg_conn_w = 6;      // Width of the connectors
dbg_conn_sl = 14;    // Short length
dbg_conn_ll = 20;    // Long length
dbg_s_px = 4;        // Short X pos from edge of board
dbg_s_py = 12.5;     // Short Y pos from top of board
dbg_l_px = 4;        // Long X pos from edge of board
dbg_l_py = 19.5;     // Long Y pos from top of board

// LEDs
led_x = 30;          // X offset of LEDs
led_y_s = 8;         // Y start position of LEDs
led_y_i = 4;         // Y interval between LEDs
led_r = 1.3;         // Radius of LED hole
led_shroud_w = 4.5+border/2;  // Width of the LED lightpipe shroud
led_shroud_l = 31;   // Length of the LED lightpipe shroud
led_h_relief = 1.25; // Height above LED before starting the lightpipe

// Pushbutton
pb_y = 3.5;          // Y Pos
pb_x = 24.5;         // X Pos
pb_r = 1;            // Radius



pcb_x_start = (outer_width-pcb_width)/2;
pcb_y_start = (outer_length-pcb_length)/2;
pcb_x_end = pcb_x_start + pcb_width-0.1;
pcb_y_end = pcb_y_start + pcb_length-0.1;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// End of libraries
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

module roundedcube(xx, yy, ht, radius)
{
    translate([0,0,ht/2])
    hull()
    {
        translate([radius,radius,0])
        cylinder(ht,radius,radius,true);

        translate([xx-radius,radius,0])
        cylinder(ht,radius,radius,true);

        translate([xx-radius,yy-radius,0])
        cylinder(ht,radius,radius,true);

        translate([radius,yy-radius,0])
        cylinder(ht,radius,radius,true);
    }
}

module CountersunkScrewHole(
	screwRadius		= 2,
	headRadius		= 3,
	headDepth		= 2,
	holeDepth		= 10,
	countersinkDepth	= 10
)
{
	translate([0, 0, -holeDepth/2])
	union(){
		// countersink
		translate([0, 0, holeDepth/2 + countersinkDepth/2])
			cylinder(h=countersinkDepth, r=headRadius, center=true, $fn=60);
		// beveled head
		translate([0, 0, holeDepth/2 - headDepth/2])
			cylinder(h=headDepth, r1=screwRadius, r2=headRadius, center=true, $fn=60);
		// screw shaft
		cylinder(h=holeDepth, r=screwRadius, center=true, $fn=60);
	} // union
} // CountersunkScrewHole module
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// End of libraries
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

module base() {
    difference() {
        union() {
        difference() {
            union () {
            roundedcube(outer_width,outer_length,base_depth,z_radius);
            translate([(outer_width-bulb_width)/2, (outer_length-bulb_length),0]) roundedcube(bulb_width,bulb_length,bulb_depth,z_radius);

            // Front lip for USB
            translate([0,-lip_extension,0]) roundedcube(outer_width,10,base_depth,z_radius);
            }

            // The PCB itself, with room above it
            translate([(outer_width-pcb_width)/2-pcb_tol, (outer_length-pcb_length)/2-pcb_tol, base_top_thickness])
            roundedcube(pcb_width+2*pcb_tol, pcb_length+2*pcb_tol, base_depth-base_top_thickness+micro,z_radius);

            // Hole for USB
            translate([(outer_width-usb_width)/2,-micro-lip_extension,pcb_tray_depth-usb_below+micro]) cube([usb_width,10+lip_extension,base_depth-pcb_tray_depth+usb_below]);
        }

        // The four corners
        translate([pcb_x_start-0.5, pcb_y_start-0.5,0]) roundedcube(pcb_support, pcb_support,pcb_tray_depth,pcb_support_z_radius);
        translate([pcb_x_end-pcb_support+0.5, pcb_y_start-0.5,0]) roundedcube(pcb_support, pcb_support,pcb_tray_depth,pcb_support_z_radius);
        translate([pcb_x_start-0.5, pcb_y_end-pcb_support+0.5,0]) roundedcube(pcb_support, pcb_support,pcb_tray_depth,pcb_support_z_radius);
        translate([pcb_x_end-pcb_support+0.5, pcb_y_end-pcb_support+0.5,0]) roundedcube(pcb_support, pcb_support,pcb_tray_depth,pcb_support_z_radius);

        // LED Shroud
        translate([0,pcb_y_start+5,0]) cube([led_shroud_w, led_shroud_l-5,pcb_tray_depth-led_h_relief]);
        }

        // Drill holes
        translate([(outer_width-pcb_width)/2 + pcb_hole_displacement,(outer_length-pcb_length)/2+pcb_hole_displacement, top_tap_offs]) rotate([0,0,90]) cylinder(r=top_tap_holesize, h=top_tap_holedepth);

        translate([(outer_width-pcb_width)/2 + pcb_hole_displacement,outer_length-(outer_length-pcb_length)/2-pcb_hole_displacement, top_tap_offs]) rotate([0,0,90]) cylinder(r=top_tap_holesize, h=top_tap_holedepth);
        translate([outer_width-(outer_width-pcb_width)/2 - pcb_hole_displacement,outer_length-(outer_length-pcb_length)/2-pcb_hole_displacement, top_tap_offs]) rotate([0,0,90]) cylinder(r=top_tap_holesize, h=top_tap_holedepth);
        translate([outer_width-(outer_width-pcb_width)/2 - pcb_hole_displacement,(outer_length-pcb_length)/2+pcb_hole_displacement, top_tap_offs]) rotate([0,0,90]) cylinder(r=top_tap_holesize, h=top_tap_holedepth);

        // Debug connectors

       translate( [ pcb_x_start + dbg_s_px, pcb_y_end - dbg_s_py, -micro] )  cube([dbg_conn_sl, dbg_conn_w, base_depth+micro]);
        translate( [ pcb_x_start + dbg_l_px, pcb_y_end - dbg_l_py, -micro] )  cube([dbg_conn_ll, dbg_conn_w, base_depth+micro]);

        // The side pins
        translate([(outer_width-bulb_width)/2-micro, outer_length - side_pins_width - side_pins_north_ofs, pcb_tray_depth-side_pins_height ])
        cube([dbg_s_px+(bulb_width-outer_width)/2+2*micro,side_pins_width+micro,base_depth-pcb_tray_depth+side_pins_height+micro]);
        translate([dbg_s_px+pcb_x_start+dbg_conn_ll+1, outer_length - side_pins_width - side_pins_north_ofs, pcb_tray_depth-side_pins_height ])
         cube([dbg_s_px+(bulb_width-outer_width)/2+micro+10,side_pins_width+micro,base_depth-pcb_tray_depth+side_pins_height+micro]);

        // LEDs
        translate([pcb_x_end-led_x,pcb_y_start+led_y_s,-micro]) rotate([0,0,90]) cylinder(r=led_r,h=10);
        translate([pcb_x_end-led_x,pcb_y_start+led_y_s+led_y_i,-micro]) rotate([0,0,90]) cylinder(r=led_r,h=10);
        translate([pcb_x_end-led_x,pcb_y_start+led_y_s+2*led_y_i,-micro]) rotate([0,0,90]) cylinder(r=led_r,h=10);
        translate([pcb_x_end-led_x,pcb_y_start+led_y_s+3*led_y_i,-micro]) rotate([0,0,90]) cylinder(r=led_r,h=10);
        translate([pcb_x_end-led_x,pcb_y_start+led_y_s+4*led_y_i,-micro]) rotate([0,0,90]) cylinder(r=led_r,h=10);
        translate([pcb_x_end-led_x,pcb_y_start+led_y_s+5*led_y_i,-micro]) rotate([0,0,90]) cylinder(r=led_r,h=10);

        // Pushbutton
        translate([pcb_x_end-pb_x,pcb_y_start+pb_y,-micro]) rotate([0,0,90]) cylinder(r=pb_r,h=10);

            // Top recess
            translate([tr_ofs,tr_ofs-lip_extension,-micro]) roundedcube(outer_width-tr_ofs*2,lip_extension+outer_length-tr_ofs*2,tr_depth,z_radius);
            translate([tr_ofs+(outer_width-bulb_width)/2, tr_ofs+(outer_length-bulb_length),-micro]) roundedcube(bulb_width-tr_ofs*2,bulb_length-tr_ofs*2,tr_depth,z_radius);

    }
}

module top() {
    difference() {
        union() {
        difference() {
            union () {
            roundedcube(outer_width,outer_length,top_depth,z_radius);
           translate([(outer_width-bulb_width)/2, (outer_length-bulb_length),0]) roundedcube(bulb_width,bulb_length,top_depth,z_radius);
            // Front lip for USB
            translate([0,-lip_extension,0]) roundedcube(outer_width,10,top_depth,z_radius);

            // Protrusion for USB
           translate([fit_delta+(outer_width-usb_width)/2,-lip_extension,top_depth-micro]) cube([usb_width-(2*fit_delta), 1+lip_extension,pcb_thickness]);

        // The side pins
        translate([(outer_width-bulb_width)/2-micro, outer_length - side_pins_width - side_pins_north_ofs+fit_delta, 0 ])
        cube([dbg_s_px+(bulb_width-outer_width)/2+micro,side_pins_width+micro-2*fit_delta,top_depth-micro+pcb_thickness]);
        translate([pcb_width+border/2+pcb_tol*2, outer_length - side_pins_width - side_pins_north_ofs+fit_delta, 0 ])
          cube([(bulb_width-pcb_width)/2-pcb_tol,side_pins_width+micro-2*fit_delta,top_depth+pcb_thickness-micro]);

            }

            // Cutout for the PCB itself, with room above it
            translate([(outer_width-pcb_width)/2-pcb_tol, (outer_length-pcb_length)/2-pcb_tol, top_top_thickness-micro])
            roundedcube(pcb_width+2*pcb_tol, pcb_length+2*pcb_tol, top_depth-base_top_thickness+2*micro+pcb_thickness,z_radius);
        }


        // The four corners
        translate([pcb_x_start-0.5, pcb_y_start-0.5,0]) roundedcube(pcb_support, pcb_support,top_depth,pcb_support_z_radius);
        translate([pcb_x_end-pcb_support+0.5, pcb_y_start-0.5,0]) roundedcube(pcb_support, pcb_support,top_depth,pcb_support_z_radius);
        translate([pcb_x_start-0.5, pcb_y_end-pcb_support+0.5,0]) roundedcube(pcb_support, pcb_support,top_depth,pcb_support_z_radius);
        translate([pcb_x_end-pcb_support+0.5, pcb_y_end-pcb_support+0.5,0]) roundedcube(pcb_support, pcb_support,top_depth,pcb_support_z_radius);

        }

        // Drill holes
        translate([(outer_width-pcb_width)/2 + pcb_hole_displacement,(outer_length-pcb_length)/2+pcb_hole_displacement, -micro]) rotate([0,180,90])
          CountersunkScrewHole(	screwRadius = 1.35, headRadius = 2.5, headDepth = 1.55,	holeDepth = top_depth+2*micro,countersinkDepth = 0 );

        translate([(outer_width-pcb_width)/2 + pcb_hole_displacement,outer_length-(outer_length-pcb_length)/2-pcb_hole_displacement, -micro]) rotate([0,180,90])
          CountersunkScrewHole(	screwRadius = 1.35, headRadius = 2.5, headDepth = 1.55,	holeDepth = top_depth+2*micro,countersinkDepth = 0 );


        translate([outer_width-(outer_width-pcb_width)/2 - pcb_hole_displacement,outer_length-(outer_length-pcb_length)/2-pcb_hole_displacement, -micro]) rotate([0,180,90])
          CountersunkScrewHole(	screwRadius = 1.35, headRadius = 2.5, headDepth = 1.55,	holeDepth = top_depth+2*micro,countersinkDepth = 0 );

        translate([outer_width-(outer_width-pcb_width)/2 - pcb_hole_displacement,(outer_length-pcb_length)/2+pcb_hole_displacement, -micro]) rotate([0,180,90])
          CountersunkScrewHole(	screwRadius = 1.35, headRadius = 2.5, headDepth = 1.55,	holeDepth = top_depth+2*micro,countersinkDepth = 0 );

    }
}


base();

translate([-10,outer_length,0]) rotate([180,180,0]) top();
