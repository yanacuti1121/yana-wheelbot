// Yana Wheelbot -- prototype integration chassis V2
//
// Functional FDM fit-test deck using the component envelopes supplied by
// the project owner on 2026-08-23. The envelope dimensions already include
// normal printing clearance; only connector/service space is added here.
// LED and power-switch packages remain TBD, so this revision provides
// universal mounting holes/blank areas instead of pretending their cutouts
// are final.

$fn = 32;

// --------------------------------------------------------------------------
// Confirmed prototype envelopes (mm)
// --------------------------------------------------------------------------
esp_l = 71; esp_w = 29; esp_h = 15;
tft_l = 57; tft_w = 35; tft_h = 10;
mic_l = 22; mic_w = 17; mic_h = 8;
amp_l = 22; amp_w = 21; amp_h = 8;
speaker_d = 42; speaker_h = 21;
tof_l = 25; tof_w = 22; tof_h = 10;
touch_l = 18; touch_w = 14; touch_h = 5;
drive_l = 25; drive_axis_l = 31; drive_h = 14;  // SG90 rotated: shaft along Y
pose_servo_l = 25; pose_servo_w = 14; pose_servo_h = 31;
battery_l = 70; battery_w = 40; battery_h = 10;
charger_l = 32; charger_w = 22; charger_h = 10;
boost_l = 42; boost_w = 24; boost_h = 12;  // universal 5V/3A-class placeholder

// --------------------------------------------------------------------------
// Print/chassis parameters
// --------------------------------------------------------------------------
deck_l = 210;
deck_w = 120;
deck_t = 3.2;
wall_t = 3;
low_wall_h = 6;
pcb_wall_h = 5;
z_fuse = 0.35;
mount_hole_d = 3.4;

// Part locations; +X is front, +Y is left.
drive_x = 52;
drive_y = deck_w / 2 - drive_axis_l / 2 - 2;
esp_x = 35;
battery_x = -45;
battery_y = -7;
speaker_x = -78;
speaker_y = 38;
arm_x = -25;
arm_y = 42.5;
neck_x = 16;
neck_y = 45;
tof_x = 89;
tof_y = -45;
charger_x = -86;
boost_x = -31;
amp_x = 7;
touch_x = 33;
mic_x = 59;
power_row_y = -45;
switch_x = 88;
switch_y = 22;

// --------------------------------------------------------------------------
// Reusable mounts
// --------------------------------------------------------------------------

// Open-top pocket. The deck itself is the floor; low walls locate the part
// without trapping heat or making removal difficult.
module pocket(l, w, h = pcb_wall_h, wall = wall_t) {
    difference() {
        cube([l + 2 * wall, w + 2 * wall, h], center = true);
        translate([0, 0, wall])
            cube([l, w, h + 1], center = true);
    }
}

// Two support rails keep PCB solder joints/header pins off the main deck.
module pcb_support_rails(l, w) {
    rail_w = 3;
    rail_h = 2.4;
    for (side = [-1, 1])
        translate([0, side * (w / 2 - rail_w / 2), rail_h / 2])
            cube([l - 4, rail_w, rail_h], center = true);
}

module pcb_mount(l, w, h = pcb_wall_h) {
    pocket(l, w, h);
    pcb_support_rails(l, w);
}

// Horizontal SG90 cradle. The original V1 stood wheel servos upright, which
// pointed the output axes vertically. V2 lays each servo on its side so its
// output axis points out toward the wheel/track along +/-Y. Both Y ends stay
// open for the horn and cable; a strap can pass through the two slots.
module horizontal_servo_cradle() {
    shelf_l = drive_l + 2 * wall_t;
    shelf_w = drive_axis_l + 3;
    shelf_t = 2.6;
    rail_h = drive_h + 3;

    cube([shelf_l, shelf_w, shelf_t], center = true);
    for (side = [-1, 1])
        translate([side * (drive_l / 2 + wall_t / 2), 0, rail_h / 2])
            cube([wall_t, shelf_w, rail_h], center = true);
}

// Upright SG90-class pocket for the neck servo. The shaft points upward.
module upright_servo_pocket() {
    pocket(pose_servo_l, pose_servo_w, 10);
}

// TFT landscape bezel. It holds the full 57x35 PCB envelope from the rear;
// the conservative window avoids assuming exact mounting-hole locations.
module tft_bezel() {
    bezel_y = tft_l + 2 * wall_t;
    bezel_z = tft_w + 2 * wall_t;
    difference() {
        cube([wall_t, bezel_y, bezel_z], center = true);
        cube([wall_t + 2, tft_l - 8, tft_w - 8], center = true);
    }

    // Rear retaining rails: bottom and two sides, open at top for insertion.
    translate([-tft_h / 2, 0, -bezel_z / 2 + wall_t / 2])
        cube([tft_h, bezel_y, wall_t], center = true);
    for (side = [-1, 1])
        translate([-tft_h / 2, side * (bezel_y / 2 - wall_t / 2), 0])
            cube([tft_h, wall_t, bezel_z], center = true);
}

module speaker_ring() {
    difference() {
        cylinder(h = low_wall_h, d = speaker_d + 2 * wall_t, center = true, $fn = 64);
        cylinder(h = low_wall_h + 2, d = speaker_d, center = true, $fn = 64);
    }
}

// Gusset for the vertical TFT bezel, printed without a floating interface.
module bezel_gusset(side) {
    hull() {
        translate([-9, side * (tft_l / 2 + wall_t / 2), -18])
            cube([18, wall_t, deck_t], center = true);
        translate([0, side * (tft_l / 2 + wall_t / 2), -5])
            cube([wall_t, wall_t, 6], center = true);
    }
}

// --------------------------------------------------------------------------
// Assembly
// --------------------------------------------------------------------------

module chassis_v2() {
    difference() {
        union() {
            // Main printable deck.
            cube([deck_l, deck_w, deck_t], center = true);

            // Correctly oriented left/right drive servos.
            for (side = [-1, 1])
                translate([drive_x, side * drive_y,
                           deck_t / 2 + 1.3 - z_fuse])
                    horizontal_servo_cradle();

            // Main ESP32-S3 board, with USB/header ends unobstructed.
            translate([esp_x, 0, deck_t / 2 + pcb_wall_h / 2 - z_fuse])
                pcb_mount(esp_l, esp_w);

            // LiPo: low retaining lip only; use a soft hook-and-loop strap.
            translate([battery_x, battery_y,
                       deck_t / 2 + low_wall_h / 2 - z_fuse])
                pocket(battery_l, battery_w, low_wall_h);

            // Power integrity is treated as a subsystem: the charger/protection
            // and regulated 5V boost have separate, independently serviceable
            // pockets. The boost envelope is intentionally universal until the
            // exact 5V/3A-class board is selected.
            translate([charger_x, power_row_y,
                       deck_t / 2 + pcb_wall_h / 2 - z_fuse])
                pcb_mount(charger_l, charger_w);
            translate([boost_x, power_row_y,
                       deck_t / 2 + pcb_wall_h / 2 - z_fuse])
                pcb_mount(boost_l, boost_w);

            // Audio and touch modules remain removable. The microphone stays
            // at the opposite front corner from the speaker to reduce feedback.
            translate([amp_x, power_row_y,
                       deck_t / 2 + pcb_wall_h / 2 - z_fuse])
                pcb_mount(amp_l, amp_w);
            translate([touch_x, power_row_y,
                       deck_t / 2 + pcb_wall_h / 2 - z_fuse])
                pcb_mount(touch_l, touch_w);
            translate([mic_x, power_row_y,
                       deck_t / 2 + pcb_wall_h / 2 - z_fuse])
                pcb_mount(mic_l, mic_w);

            // Front downward-looking ToF pocket; optical opening is cut below.
            translate([tof_x, tof_y,
                       deck_t / 2 + pcb_wall_h / 2 - z_fuse])
                pcb_mount(tof_l, tof_w);

            // Speaker lies cone-up over a grille. Magnet/depth extends upward.
            translate([speaker_x, speaker_y,
                       deck_t / 2 + low_wall_h / 2 - z_fuse])
                speaker_ring();

            // Arm servo lies down with its output shaft toward +Y.
            translate([arm_x, arm_y,
                       deck_t / 2 + 1.3 - z_fuse])
                horizontal_servo_cradle();

            // Neck servo stands upright; horn/neck attaches above it.
            translate([neck_x, neck_y,
                       deck_t / 2 + 5 - z_fuse])
                upright_servo_pocket();

            // Landscape TFT at the front edge.
            bezel_z = tft_w + 2 * wall_t;
            translate([deck_l / 2 - wall_t / 2, 0,
                       deck_t / 2 + bezel_z / 2 - z_fuse]) {
                tft_bezel();
                bezel_gusset(-1);
                bezel_gusset(1);
            }
        }

        // ToF optical path: generous 14mm window, centered on the breakout.
        translate([tof_x, tof_y, 0])
            cylinder(h = deck_t + 12, d = 14, center = true);

        // Speaker grille: center plus two rings of holes through the deck.
        translate([speaker_x, speaker_y, 0]) {
            cylinder(h = deck_t + 2, d = 4, center = true);
            for (radius = [8, 15])
                for (angle = [0 : 45 : 315])
                    rotate([0, 0, angle])
                        translate([radius, 0, 0])
                            cylinder(h = deck_t + 2, d = 3, center = true, $fn = 20);
        }

        // Four chassis/body attachment holes.
        for (x = [-1, 1])
            for (y = [-1, 1])
                translate([x * (deck_l / 2 - 7), y * (deck_w / 2 - 7), 0])
                    cylinder(h = deck_t + 4, d = mount_hole_d, center = true);

        // Universal rear caster/skid slots; ignore when using Wall-E tracks.
        for (y = [-12, 12])
            translate([-94, y, 0])
                hull() {
                    translate([-3, 0, 0])
                        cylinder(h = deck_t + 4, d = mount_hole_d, center = true);
                    translate([3, 0, 0])
                        cylinder(h = deck_t + 4, d = mount_hole_d, center = true);
                }

        // Cable-pass slots near the major zones.
        for (point = [[72, 0], [0, 0], [-18, -28], [-83, -18]])
            translate([point[0], point[1], 0])
                hull() {
                    translate([-4, 0, 0])
                        cylinder(h = deck_t + 4, d = 4, center = true);
                    translate([4, 0, 0])
                        cylinder(h = deck_t + 4, d = 4, center = true);
                }

        // Battery strap slots sit outside the battery pocket so the cell is
        // removable without relying on permanent adhesive.
        for (x = [battery_x - battery_l / 2 - 6,
                  battery_x + battery_l / 2 + 6])
            translate([x, battery_y, 0])
                hull() {
                    translate([0, -5, 0])
                        cylinder(h = deck_t + 4, d = 3.5, center = true);
                    translate([0, 5, 0])
                        cylinder(h = deck_t + 4, d = 3.5, center = true);
                }

        // Universal mini-switch tie slots. They are top-accessible but avoid
        // assuming the final slide-switch body or panel-cutout dimensions.
        for (x = [switch_x - 7, switch_x + 7])
            translate([x, switch_y, 0])
                hull() {
                    translate([0, -2, 0])
                        cylinder(h = deck_t + 4, d = 2.8, center = true);
                    translate([0, 2, 0])
                        cylinder(h = deck_t + 4, d = 2.8, center = true);
                }

        // No LED socket is cut until the actual LED body is measured.
    }
}

chassis_v2();
