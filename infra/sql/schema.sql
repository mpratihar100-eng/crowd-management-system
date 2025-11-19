-- Crowd Management System Database Schema

CREATE EXTENSION IF NOT EXISTS "uuid-ossp";

-- Cameras table
CREATE TABLE cameras (
    camera_id VARCHAR(64) PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    location VARCHAR(255),
    rtsp_url VARCHAR(512),
    homography_matrix DOUBLE PRECISION[9],
    active BOOLEAN DEFAULT true,
    created_at TIMESTAMP DEFAULT NOW(),
    updated_at TIMESTAMP DEFAULT NOW()
);

-- Zones table
CREATE TABLE zones (
    zone_id VARCHAR(64) PRIMARY KEY,
    name VARCHAR(255) NOT NULL,
    min_x DOUBLE PRECISION NOT NULL,
    min_y DOUBLE PRECISION NOT NULL,
    max_x DOUBLE PRECISION NOT NULL,
    max_y DOUBLE PRECISION NOT NULL,
    area_m2 DOUBLE PRECISION NOT NULL,
    threshold_high INTEGER DEFAULT 60,
    threshold_critical INTEGER DEFAULT 100,
    active BOOLEAN DEFAULT true,
    created_at TIMESTAMP DEFAULT NOW()
);

-- Zone metrics
CREATE TABLE zone_metrics (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    zone_id VARCHAR(64) NOT NULL REFERENCES zones(zone_id),
    timestamp TIMESTAMP NOT NULL DEFAULT NOW(),
    people_count INTEGER NOT NULL,
    density DOUBLE PRECISION NOT NULL,
    trend DOUBLE PRECISION
);

CREATE INDEX idx_zone_metrics_timestamp ON zone_metrics(timestamp DESC);
CREATE INDEX idx_zone_metrics_zone ON zone_metrics(zone_id, timestamp DESC);

-- Heatmap tiles
CREATE TABLE heatmap_tiles (
    tile_x INTEGER NOT NULL,
    tile_y INTEGER NOT NULL,
    heat_value DOUBLE PRECISION NOT NULL,
    last_update TIMESTAMP DEFAULT NOW(),
    PRIMARY KEY (tile_x, tile_y)
);

-- Recommendations
CREATE TABLE recommendations (
    id VARCHAR(64) PRIMARY KEY,
    zone_id VARCHAR(64) NOT NULL REFERENCES zones(zone_id),
    type VARCHAR(32) NOT NULL,
    priority VARCHAR(16) NOT NULL,
    message TEXT NOT NULL,
    timestamp TIMESTAMP DEFAULT NOW(),
    people_count INTEGER,
    threshold DOUBLE PRECISION,
    trend DOUBLE PRECISION,
    density DOUBLE PRECISION,
    staff_count INTEGER,
    status VARCHAR(16) DEFAULT 'pending',
    action_timestamp TIMESTAMP,
    CHECK (type IN ('reroute_staff', 'preposition_staff')),
    CHECK (priority IN ('low', 'medium', 'high', 'critical')),
    CHECK (status IN ('pending', 'accepted', 'rejected'))
);

CREATE INDEX idx_recommendations_status ON recommendations(status, timestamp DESC);

-- Detection events
CREATE TABLE detection_events (
    id UUID PRIMARY KEY DEFAULT uuid_generate_v4(),
    camera_id VARCHAR(64) NOT NULL REFERENCES cameras(camera_id),
    timestamp TIMESTAMP NOT NULL DEFAULT NOW(),
    frame_id VARCHAR(64),
    detection_count INTEGER NOT NULL,
    detections JSONB
);

CREATE INDEX idx_detection_events_timestamp ON detection_events(timestamp DESC);
CREATE INDEX idx_detection_events_camera ON detection_events(camera_id, timestamp DESC);

-- Insert sample data
INSERT INTO cameras (camera_id, name, location, active) VALUES
('cam-001', 'Main Entrance Camera', 'North entrance', true),
('cam-002', 'Ride Queue Camera 1', 'Roller coaster', true),
('cam-003', 'Food Court Camera', 'Central plaza', true);

INSERT INTO zones (zone_id, name, min_x, min_y, max_x, max_y, area_m2, threshold_high) VALUES
('zone-entrance-1', 'Main Entrance', 0, 0, 50, 30, 1500, 40),
('zone-ride-1', 'Roller Coaster Queue', 50, 0, 100, 40, 2000, 60),
('zone-food-1', 'Food Court', 100, 0, 150, 50, 2500, 80),
('zone-exit-1', 'Main Exit', 150, 0, 200, 30, 1500, 40);

-- Insert sample metrics
INSERT INTO zone_metrics (zone_id, people_count, density, trend) VALUES
('zone-entrance-1', 42, 0.028, 0.15),
('zone-ride-1', 67, 0.034, 0.25),
('zone-food-1', 55, 0.022, 0.10);

-- Insert sample recommendation
INSERT INTO recommendations (id, zone_id, type, priority, message, people_count, threshold, trend, density, staff_count) VALUES
('rec-001', 'zone-entrance-1', 'reroute_staff', 'high', 'Dispatch 2 staff to entrance - high density detected', 42, 40, 0.15, 0.028, 2);
