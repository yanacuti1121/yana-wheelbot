/*
 * Yana Wheelbot replacement eye housing for an HC-SR04-class module.
 *
 * Coordinate system:
 *   X = left/right, Y = up/down, Z = front/back.
 * Print the front housing with its face on the build plate. Print the back
 * cover with its outside face on the build plate.
 */

$fn = 96;

part = "front"; // "front", "back", or "assembly"

// HC-SR04 envelope. Measure the purchased clone before final production.
pcb_width = 45.0;
pcb_height = 20.0;
pcb_clearance = 0.6; // total width/height clearance: +0.3 mm per side
pcb_depth_from_front = 11.4;

transducer_diameter = 16.0;
transducer_clearance = 0.7;
transducer_spacing = 26.0;

// Printed shell dimensions tuned for a 0.4 mm nozzle.
pod_width = 31.0;
pod_height = 30.0;
housing_depth = 18.0;
back_cover_thickness = 2.4;
boss_hole_diameter = 2.2; // M2 thread-forming screw
boss_outer_diameter = 5.4;
boss_x = 25.5;

wire_notch_width = 8.0;
wire_notch_height = 5.0;

module_cavity_width = pcb_width + pcb_clearance;
module_cavity_height = pcb_height + pcb_clearance;
transducer_hole_diameter = transducer_diameter + transducer_clearance;


module rounded_rect_2d(width, height, radius) {
    offset(r = radius)
        square([width - 2 * radius, height - 2 * radius], center = true);
}


module pod_outline_2d() {
    union() {
        for (x = [-transducer_spacing / 2, transducer_spacing / 2])
            translate([x, 0])
                scale([pod_width / 2, pod_height / 2])
                    circle(r = 1);

        // A hidden rear bridge keeps both pods mechanically tied together.
        rounded_rect_2d(module_cavity_width + 4.8, module_cavity_height + 5.8, 3.0);
    }
}


module front_housing() {
    difference() {
        union() {
            linear_extrude(height = housing_depth)
                pod_outline_2d();

            // Side screw bosses sit just beyond the PCB's left/right edges.
            for (x = [-boss_x, boss_x])
                translate([x, 0, pcb_depth_from_front])
                    cylinder(d = boss_outer_diameter, h = housing_depth - pcb_depth_from_front);
        }

        // Openings for the two ultrasonic cans.
        for (x = [-transducer_spacing / 2, transducer_spacing / 2])
            translate([x, 0, -0.1])
                cylinder(d = transducer_hole_diameter, h = pcb_depth_from_front + 0.5);

        // Rear-loading pocket for the PCB and its components.
        translate([0, 0, pcb_depth_from_front + (housing_depth - pcb_depth_from_front + 0.4) / 2])
            cube(
                [module_cavity_width, module_cavity_height, housing_depth - pcb_depth_from_front + 0.4],
                center = true
            );

        // Bottom exit for the four sensor wires.
        translate([
            -wire_notch_width / 2,
            -pod_height / 2 - 0.2,
            pcb_depth_from_front
        ])
            cube([wire_notch_width, wire_notch_height + 0.3, housing_depth - pcb_depth_from_front + 0.5]);

        for (x = [-boss_x, boss_x])
            translate([x, 0, pcb_depth_from_front - 0.1])
                cylinder(d = boss_hole_diameter, h = housing_depth - pcb_depth_from_front + 0.4);
    }
}


module neck_mount_tab_2d() {
    translate([0, -18.1])
        rounded_rect_2d(22.0, 8.0, 2.0);
}


module back_cover() {
    difference() {
        union() {
            linear_extrude(height = back_cover_thickness)
                union() {
                    pod_outline_2d();
                    neck_mount_tab_2d();
                }

            // Two shallow locating rails sit above and below the PCB.
            for (y = [-11.35, 11.35])
                translate([0, y, back_cover_thickness + 1.1])
                    cube([41.5, 1.6, 2.2], center = true);
        }

        for (x = [-boss_x, boss_x])
            translate([x, 0, -0.1])
                cylinder(d = boss_hole_diameter + 0.4, h = back_cover_thickness + 3.5);

        // Universal SG90 horn/neck pattern in the external lower tab.
        for (x = [-6, 0, 6])
            translate([x, -18.1, -0.1])
                cylinder(d = x == 0 ? 2.5 : 2.0, h = back_cover_thickness + 0.3);

        translate([
            -wire_notch_width / 2,
            -pod_height / 2 - 0.2,
            -0.1
        ])
            cube([wire_notch_width, wire_notch_height + 0.3, back_cover_thickness + 3.5]);
    }
}


if (part == "front") {
    front_housing();
} else if (part == "back") {
    back_cover();
} else if (part == "assembly") {
    color("gold") front_housing();
    color("dimgray")
        translate([0, 0, housing_depth + back_cover_thickness + 0.2])
            rotate([180, 0, 0])
                back_cover();
} else {
    assert(false, str("Unknown part: ", part));
}
