import json
import tarfile
import os
import tempfile
import configparser
import sys

def read_properties_file(path):
    config = configparser.RawConfigParser()
    config.read(path)
    return config

def get_manifest(appPath):
    """Extract only the file named 'manifest.properties' from the given tar/tar.gz
    archive into the directory specified by outputPath.

    Returns True on success, False on failure.
    """
    print(f"Unpacking manifest.properties from {appPath}")
    if not os.path.isfile(appPath):
        print(f"File not found: {appPath}")
        return False

    try:
        # Auto-detect compression (tar, tar.gz, tar.bz2, etc.)
        with tarfile.open(appPath, mode="r:*") as tf:
            # Find a member whose base name is exactly 'manifest.properties'
            member = None
            for m in tf.getmembers():
                # Skip directories and non-files
                if not m.isfile():
                    continue
                if os.path.basename(m.name) == "manifest.properties":
                    member = m
                    break

            if member is None:
                print("manifest.properties not found in archive")
                return None

            # Read the member's content and write it directly into outputPath
            # Create a temporary directory that persists after this function returns.
            temp_dir = tempfile.mkdtemp(prefix="tactility_app_")
            dest_file = os.path.join(temp_dir, "manifest.properties")
            with tf.extractfile(member) as src, open(dest_file, "wb") as dst:
                if src is None:
                    print("Failed to open manifest.properties from the archive")
                    return False
                while True:
                    chunk = src.read(1024 * 1024)
                    if not chunk:
                        break
                    dst.write(chunk)
            properties = read_properties_file(dest_file)
            os.remove(dest_file)
            os.rmdir(temp_dir)
            return properties
    except tarfile.ReadError as e:
        print(f"Not a valid tar archive: {appPath} ({e})")
    except Exception as e:
        print(f"Failed to extract manifest.properties from {appPath}: {e}")
    return None

def get_versioned_file_name(manifest):
    app_id = manifest["app"]["id"]
    version_code = manifest["app"]["versionCode"]
    return f"{app_id}-{version_code}.app"

def get_os_version(manifest):
    sdk = manifest["target"]["sdk"]
    # Remove trailing hyphen suffix if present
    if "-" in sdk:
        return sdk.rsplit("-", 1)[0].strip()
    else:
        return sdk

def manifest_config_to_flat_json(manifest):
    """Convert a ConfigParser manifest into a flat JSON-like dict.

    Expected sections/keys (case-insensitive for keys):
    - [app]
        id -> appId
        versionName -> appVersionName
        versionCode -> appVersionCode (int)
        name -> appName
        description -> appDescription (optional; default "")
    - [target]
        sdk -> targetSdk
        platforms -> targetPlatforms (comma-separated list)

    Unknown/missing values fall back to sensible defaults per requirements.
    """
    def get_opt(section, option, default=None):
        if not manifest.has_section(section):
            return default
        # try exact option then lowercase (RawConfigParser lowercases by default)
        if manifest.has_option(section, option):
            return manifest.get(section, option)
        low = option.lower()
        if manifest.has_option(section, low):
            return manifest.get(section, low)
        return default

    # Map values
    app_id = get_opt("app", "id", "")
    app_version_name = get_opt("app", "versionName", "")
    app_version_code_raw = get_opt("app", "versionCode", "0")
    app_name = get_opt("app", "name", "")
    app_description = get_opt("app", "description", "") or ""

    # Coerce version code to int safely
    try:
        app_version_code = int(str(app_version_code_raw).strip())
    except Exception:
        app_version_code = 0

    target_sdk = get_opt("target", "sdk", "")
    platforms_raw = get_opt("target", "platforms", "")
    target_platforms = [p.strip() for p in str(platforms_raw).split(",") if p.strip()] if platforms_raw is not None else []

    filename = get_versioned_file_name(manifest)
    return {
        "appId": app_id,
        "appVersionName": app_version_name,
        "appVersionCode": app_version_code,
        "appName": app_name,
        "appDescription": app_description,
        "targetSdk": target_sdk,
        "targetPlatforms": target_platforms,
        "file": filename
    }

def write_json(filepath, data):
    text = json.dumps(data)
    with open(filepath, 'w') as f:
        f.write(text)

if __name__ == "__main__":
    print("Tactility app release script")
    if len(sys.argv) != 2:
        print("Missing argument")
        sys.exit()
    app_directory = sys.argv[1]
    manifest_map = {}
    output_json = {
        "apps": []
    }
    any_manifest = None
    if os.path.exists(app_directory):
        for file in os.listdir(app_directory):
            if file.endswith(".app"):
                file_path = os.path.join(app_directory, file)
                manifest_map[file_path] = get_manifest(file_path)
    # Rename files and collect manifest data into output json object
    for file_path in manifest_map.keys():
        print(f"Processing {file_path}: {manifest_map[file_path]}")
        manifest = manifest_map[file_path]
        manifest_json = manifest_config_to_flat_json(manifest)
        output_json["apps"].append(manifest_json)
        release_file_name = get_versioned_file_name(manifest)
        renamed_file_path = os.path.join(app_directory, release_file_name)
        os.rename(file_path, renamed_file_path)
        print(f"Renamed {file_path} to {renamed_file_path}")
        any_manifest = manifest
    # Write JSON
    output_json_path = os.path.join(app_directory, "apps.json")
    write_json(output_json_path, output_json)