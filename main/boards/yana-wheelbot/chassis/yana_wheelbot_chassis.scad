// Yana Wheelbot -- functional mounting chassis (not a cosmetic outer shell)
//
// PURPOSE: This solves "where does each real component physically go and how
// is it held down" -- it is a flat base deck with mounting bays/standoffs for
// every part in this board's README parts list, not a finished enclosure.
// A cosmetic outer cover (the Wall-E-style look, etc.) is a separate later
// step once the real parts are in hand and fit is confirmed on this deck.
//
// HOW TO USE / ADJUST: every dimension is a named variable in the "PARTS
// DIMENSIONS" section below. Several are real, sourced measurements (cited
// inline); others are common/ballpark sizes for parts that vary by seller
// (marked ADJUST). Measure your actual purchased parts and edit the
// matching variable before printing -- do not trust these numbers blindly.
//
// Render a preview:   openscad -o preview.png yana_wheelbot_chassis.scad
// Export for printing: openscad -o chassis.stl yana_wheelbot_chassis.scad
//
// Matches main/boards/yana-wheelbot/config.h's default (servo-driven wheels)
// motor backend and the parts list in this board's README.md.

// ============================================================
// PARTS DIMENSIONS
// ============================================================

// -- SG90-class continuous-rotation servo (wheel, arm, neck all use this
//    same body size) -- body dims cross-checked against multiple TowerPro
//    SG90 datasheets/resellers (23.0 x 12.2 x 29.0mm, +-1-2mm across
//    sources). Mounting-tab spread (32.5mm) and tab screw-hole spacing
//    (28mm) are the long-standing SG90/MG90S-family standard.
servo_body_l   = 23.0;
servo_body_w   = 12.2;
servo_body_h   = 29.0;
servo_tab_span = 32.5;   // outer edge to outer edge of the mounting tabs
servo_tab_hole_spacing = 28.0;
servo_tab_hole_d = 2.2;  // clearance for M2 screw

// -- ESP32-S3 dev board -- ADJUST: dev board footprints vary a lot by
//    vendor. Placeholder sized for a common ~54x28mm dev board footprint
//    (e.g. many ESP32-S3-DevKitC-1-style boards land near this). Standoff
//    holes assume a common 2-hole-per-side pattern with 3mm clearance
//    holes; re-measure your actual board before drilling/printing final.
mcu_board_l = 54.0;   // ADJUST to your actual board
mcu_board_w = 28.0;   // ADJUST to your actual board
mcu_standoff_h = 6.0;
mcu_standoff_d = 5.0;
mcu_standoff_hole_d = 2.6;  // clearance for M2.5 self-tap standoff screw

// -- 1S LiPo 2000mAh pouch battery -- ~49-50 x 34 x 10mm is the commonly
//    cited size for this capacity/format (cross-checked across several
//    hobby-battery listings); add printing clearance below.
battery_l = 50.0;
battery_w = 34.0;
battery_h = 10.0;
battery_clearance = 1.5;  // extra room on each side so the pouch isn't wedged

// -- ToF sensor breakout (VL53L0X/VL6180X common small breakout board) --
//    ADJUST: breakout board size varies by seller; placeholder for a
//    common small breakout (~20 x 10mm active area on the board).
tof_board_l = 20.0;  // ADJUST to your actual breakout
tof_board_w = 10.0;  // ADJUST to your actual breakout
tof_pocket_depth = 4.0;

// -- TTP223 touch module -- ADJUST: common breakout ~14 x 8mm.
touch_board_l = 14.0;  // ADJUST to your actual module
touch_board_w = 8.0;   // ADJUST to your actual module

// -- Small speaker (e.g. 3W/4 ohm round speaker per README parts list) --
//    ADJUST: common small round speakers are 28mm or 40mm diameter; using
//    28mm as the smaller/safer default -- re-check your actual speaker.
speaker_diameter = 28.0;  // ADJUST to your actual speaker
speaker_depth = 8.0;      // ADJUST to your actual speaker

// -- Deck / wall thickness --
wall_t = 3.0;
deck_t = 3.0;

// Small guaranteed overlap so every bay/standoff genuinely fuses with the
// deck solid in the union below, instead of merely touching it at an
// exact shared face (touching-only faces are unreliable to union: CGAL
// can leave them as separate volumes instead of one fused solid).
z_fuse = 0.4;

// -- Layout: 4 zones stacked front-to-back along X, each sized from its
//    own contents' real footprint plus a gap, so none can overlap the
//    next regardless of what the individual part sizes above are set to.
//    (Servo bays are placed within the MCU zone's X-band but pushed out
//    to the Y-edges, so they don't compete with this front-to-back stack.)
zone_gap = 8.0;

tof_zone_len     = tof_board_l + 8;
mcu_zone_len     = max(mcu_board_l + 8, servo_body_l + 2 * wall_t + 8);
battery_zone_len = battery_l + 2 * battery_clearance + 2 * wall_t;
rear_zone_len    = max(speaker_diameter, touch_board_l) + 2 * wall_t;

deck_l = tof_zone_len + mcu_zone_len + battery_zone_len + rear_zone_len + 4 * zone_gap;

// Rear corner needs to fit BOTH the speaker mount and the touch pocket
// side by side, each with its own real footprint plus a gap between them.
rear_corner_w = speaker_diameter + 2 * wall_t + touch_board_w + 2 * wall_t + zone_gap;
deck_w = max(servo_tab_span + 2 * wall_t + 20, battery_w + 2 * wall_t + 20, rear_corner_w);

// Zone boundaries along X, front (+X) to back (-X), each a contiguous,
// non-overlapping band -- every part below is placed strictly inside its
// own zone's band, so cross-zone collisions are structurally impossible.
front_edge = deck_l / 2;
tof_zone_x     = front_edge - tof_zone_len / 2;
mcu_zone_x     = front_edge - tof_zone_len - zone_gap - mcu_zone_len / 2;
battery_zone_x = front_edge - tof_zone_len - zone_gap - mcu_zone_len - zone_gap - battery_zone_len / 2;
rear_zone_x    = front_edge - tof_zone_len - zone_gap - mcu_zone_len - zone_gap - battery_zone_len - zone_gap - rear_zone_len / 2;

// ============================================================
// MODULES
// ============================================================

// A single SG90-class servo bay: an open-top pocket sized to the servo
// body, with two printed ears carrying the standard tab hole spacing so
// the servo's own mounting tabs can be screwed straight to the deck.
module servo_bay(clearance = 0.6) {
    pocket_l = servo_body_l + clearance * 2;
    pocket_w = servo_body_w + clearance * 2;
    difference() {
        union() {
            // pocket walls
            cube([pocket_l + 2 * wall_t, pocket_w + 2 * wall_t, servo_body_h], center = true);
        }
        // hollow out the pocket, open at top and one end (servo horn side)
        translate([0, 0, wall_t])
            cube([pocket_l, pocket_w, servo_body_h], center = true);
    }
    // mounting-tab ears with screw holes, centered on the pocket
    for (side = [-1, 1]) {
        translate([side * servo_tab_hole_spacing / 2, 0, -servo_body_h / 2 + 1])
            difference() {
                cylinder(h = 2, d = servo_tab_hole_d + 4, center = false, $fn = 24);
                cylinder(h = 4, d = servo_tab_hole_d, center = true, $fn = 24);
            }
    }
}

// Standoff post for the MCU dev board (heat-set-insert-friendly hole).
module mcu_standoff() {
    difference() {
        cylinder(h = mcu_standoff_h, d = mcu_standoff_d, $fn = 24);
        translate([0, 0, -0.5])
            cylinder(h = mcu_standoff_h + 1, d = mcu_standoff_hole_d, $fn = 24);
    }
}

// Battery bay: a shallow open-top tray sized with clearance, plus a small
// lip so the pouch cell can't slide out sideways.
module battery_bay() {
    tray_l = battery_l + 2 * battery_clearance;
    tray_w = battery_w + 2 * battery_clearance;
    tray_h = battery_h + 2;
    difference() {
        cube([tray_l + 2 * wall_t, tray_w + 2 * wall_t, tray_h], center = true);
        translate([0, 0, wall_t])
            cube([tray_l, tray_w, tray_h], center = true);
    }
}

// Downward-facing ToF sensor pocket, mounted at the front edge of the deck
// so the sensor looks straight down at the floor just ahead of the front
// wheels -- this is what makes it a genuine anti-fall/cliff sensor; if it's
// tucked further back under the chassis body it will not see an edge until
// the wheels are already over it.
//
// NOTE: this module only returns the solid pocket shell (open-top cavity
// for the sensor board). The sight hole that lets the sensor actually see
// the floor is cut separately, through the WHOLE assembled chassis (deck
// included), by tof_sight_hole() below -- cutting it only through this
// add-on block would leave solid deck material blocking the sensor's view.
module tof_pocket() {
    difference() {
        cube([tof_board_l + 6, tof_board_w + 6, tof_pocket_depth + wall_t], center = true);
        translate([0, 0, wall_t])
            cube([tof_board_l + 2, tof_board_w + 2, tof_pocket_depth + 1], center = true);
    }
}

// The actual through-hole for tof_pocket() above -- subtract this from the
// final union of (deck + all add-ons), not from tof_pocket() alone.
module tof_sight_hole() {
    // Deliberately generous height, not trimmed to the exact pocket height
    // above -- this only needs to guarantee full penetration through the
    // deck and the pocket sitting on top of it, precision doesn't matter.
    cylinder(h = 40, d = min(tof_board_l, tof_board_w) - 2, center = true, $fn = 32);
}

// Speaker mount: a shallow recess plus a ring of small holes as a grille,
// facing forward/up depending on where it's placed in the assembly below.
module speaker_mount() {
    difference() {
        cylinder(h = speaker_depth + wall_t, d = speaker_diameter + 2 * wall_t, $fn = 48);
        translate([0, 0, wall_t])
            cylinder(h = speaker_depth + 1, d = speaker_diameter, $fn = 48);
        // grille holes
        for (a = [0 : 30 : 330]) {
            rotate([0, 0, a])
                translate([speaker_diameter / 4, 0, -1])
                    cylinder(h = wall_t + 2, d = 2.5, $fn = 16);
        }
    }
}

// Touch sensor (TTP223) mounting pocket -- shallow, open-top, sized with a
// little clearance so it can be glued or pressed in.
module touch_pocket() {
    difference() {
        cube([touch_board_l + 2 * wall_t, touch_board_w + 2 * wall_t, wall_t + 3], center = true);
        translate([0, 0, wall_t])
            cube([touch_board_l + 1, touch_board_w + 1, 4], center = true);
    }
}

// ============================================================
// ASSEMBLY -- top-level deck layout
// ============================================================
// Layout (top view, +X = front where the ToF sensor and touch pad sit):
//
//   [ToF pocket]              front edge
//   [servo L]  [MCU standoffs]  [servo R]
//   [touch pocket]  [battery bay]  [speaker mount]
//                              rear edge

module chassis() {
    difference() {
        union() {
            // base deck
            cube([deck_l, deck_w, deck_t], center = true);

            // Front-edge ToF pocket, centered in its own zone. tof_pocket()
            // is built centered on its own Z axis (cube(center=true)), so
            // its bottom sits half its own height below its origin -- add
            // that half-height here to actually rest the bottom on the
            // deck's top surface (with the usual z_fuse overlap), instead
            // of burying half the block below the deck.
            tof_pocket_h = tof_pocket_depth + wall_t;
            translate([tof_zone_x, 0, deck_t / 2 + tof_pocket_h / 2 - z_fuse])
                tof_pocket();

            // Two wheel-servo bays, symmetric left/right, pushed out to the
            // deck's Y-edges within the MCU zone's X-band -- they don't
            // compete with the front-to-back zone stack, only with the MCU
            // standoffs' Y-position (kept close to center, clear of these).
            servo_y_offset = deck_w / 2 - servo_body_w / 2 - wall_t - 2;
            for (side = [-1, 1]) {
                translate([mcu_zone_x, side * servo_y_offset, deck_t / 2 + servo_body_h / 2 - z_fuse])
                    servo_bay();
            }

            // MCU standoffs, 2x2 pattern sized to the placeholder board
            // footprint -- re-space these to match your actual board's
            // mounting holes.
            for (x = [-1, 1]) {
                for (y = [-1, 1]) {
                    translate([mcu_zone_x, x * (mcu_board_w / 2 - 4), deck_t / 2 - z_fuse])
                        translate([y * (mcu_board_l / 2 - 4), 0, 0])
                            mcu_standoff();
                }
            }

            // Battery bay, in its own zone for a low, central-rear center
            // of gravity. Same center=true compensation as tof_pocket above.
            battery_bay_h = battery_h + 2;
            translate([battery_zone_x, 0, deck_t / 2 + battery_bay_h / 2 - z_fuse])
                battery_bay();

            // Rear zone: touch pocket and speaker mount sit side by side
            // along Y, each sized from its own footprint with a real gap
            // between them, so they cannot overlap (deck_w was computed
            // above specifically to fit both here, see rear_corner_w).
            rear_touch_y = deck_w / 2 - (touch_board_w + 2 * wall_t) / 2;
            rear_speaker_y = rear_touch_y - (touch_board_w + 2 * wall_t) / 2 - zone_gap
                             - speaker_diameter / 2 - wall_t;

            // Same center=true compensation as tof_pocket above.
            touch_pocket_h = wall_t + 3;
            translate([rear_zone_x, rear_touch_y, deck_t / 2 + touch_pocket_h / 2 - z_fuse])
                touch_pocket();

            translate([rear_zone_x, rear_speaker_y, deck_t / 2 - z_fuse])
                speaker_mount();
        }

        // Sight hole for the ToF sensor, cut through the WHOLE assembly
        // above (deck included) -- see tof_sight_hole()'s own comment for
        // why this can't be done inside tof_pocket() alone.
        translate([tof_zone_x, 0, 0])
            tof_sight_hole();
    }
}

chassis();

// Arm/neck servo bays are intentionally NOT placed on this base deck --
// they mount to whatever upper body/cover structure sits above this deck
// (arm at shoulder height, neck at the display's base), which doesn't
// exist yet since the cosmetic cover is a separate later step. Reuse
// servo_bay() for both when that structure is designed.
