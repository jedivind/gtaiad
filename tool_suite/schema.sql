BEGIN TRANSACTION;

--CREATE TABLE asset_owners(
--    owner_id string,
--    asset_id int,
--    role string,
--    name string,
--    foreign key(asset_id) references assets_bldg(asset_id));

--CREATE TABLE assets_bldg(
--    asset_id int primary key,
--    floor_num int,
--    room_num int,
--    xpos int,
--    ypos int,
--    type string,
--    description string);

-- Contains the location names and x,y position on the floor map.
-- A future extension would use coordinates independent of the image such
-- as WGS84.
CREATE TABLE capture_locations (
    capture_location_name VARCHAR(20) PRIMARY KEY UNIQUE NOT NULL,
    capture_location_floor INTEGER NOT NULL,
    capture_location_x_pos INTEGER NOT NULL,
    capture_location_y_pos INTEGER NOT NULL
    -- date...perhaps????
    );

-- Contains the median power of the various AP beacons captured at a
-- single location over at least 20 seconds.
CREATE TABLE capture_fingerprints (
    capture_location_name VARCHAR(20) NOT NULL,
    capture_fingerprint_bssid VARCHAR(20) NOT NULL,
    capture_fingerprint_channel INTEGER NOT NULL,
    capture_fingerprint_median_power INTEGER NOT NULL,
    capture_fingerprint_ssid VARCHAR(500) NOT NULL,

    FOREIGN KEY (capture_location_name) REFERENCES capture_locations(capture_location_name),
    PRIMARY KEY (capture_location_name, capture_fingerprint_bssid, capture_fingerprint_channel)
    );

COMMIT;
