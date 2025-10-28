import os
import re
import json

def find_gdml_files(start_file, base_dir='geometry'):
    """
    Recursively find all GDML files starting from a root file using regex.
    """
    files_to_parse = [start_file]
    parsed_files = set()
    all_files = []

    while files_to_parse:
        current_file_path = files_to_parse.pop(0)

        # Create a relative path from the base_dir
        if not os.path.isabs(current_file_path):
            current_file_full_path = os.path.join(base_dir, os.path.basename(current_file_path))
        else:
            current_file_full_path = current_file_path

        if current_file_full_path in parsed_files:
            continue

        if not os.path.exists(current_file_full_path):
            # Fallback to searching in the root geometry folder
            current_file_full_path = os.path.join('geometry', os.path.basename(current_file_path))
            if not os.path.exists(current_file_full_path):
                print(f"Warning: File not found: {current_file_path}")
                continue

        all_files.append(current_file_full_path)
        parsed_files.add(current_file_full_path)

        try:
            with open(current_file_full_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()

            # Find included files
            include_regex = re.compile(r'<file\s+name="([^"]+\.gdml)"\s*/>')
            for match in include_regex.finditer(content):
                included_file = match.group(1)
                # Assume included files are relative to the 'geometry' directory
                files_to_parse.append(included_file)

        except Exception as e:
            print(f"Error processing file {current_file_full_path}: {e}")

    return all_files

def find_detectors_in_file(filepath):
    """
    Finds detector information in a single GDML file using regex.
    """
    detectors = []
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            content = f.read()

        # Regex to find volume blocks
        volume_regex = re.compile(r'<volume\s+name="(?P<name>[^"]+)"\s*.*?>(?P<content>.*?)</volume>', re.DOTALL)

        for volume_match in volume_regex.finditer(content):
            volume_name = volume_match.group('name')
            volume_content = volume_match.group('content')

            sens_det_match = re.search(r'<auxiliary\s+auxtype="SensDet"\s+auxvalue="[^"]+"\s*/>', volume_content)
            det_no_match = re.search(r'<auxiliary\s+auxtype="DetNo"\s+auxvalue="(?P<id>[^"]+)"\s*/>', volume_content)

            if sens_det_match and det_no_match:
                det_name = volume_name.replace('_logic', '')
                det_id = det_no_match.group('id')

                # Find line number
                line_no = content.count('\n', 0, volume_match.start()) + 1

                detectors.append({
                    'name': det_name,
                    'id': det_id,
                    'file': filepath,
                    'line': line_no
                })
    except Exception as e:
        print(f"Error processing file {filepath}: {e}")

    return detectors

if __name__ == "__main__":
    # We need a comprehensive list of all gdml files, let's find them all first
    all_gdml_files = []
    for root, dirs, files in os.walk('.'):
        for file in files:
            if file.endswith('.gdml'):
                all_gdml_files.append(os.path.join(root, file))

    all_detectors = []
    for gdml_file in all_gdml_files:
        all_detectors.extend(find_detectors_in_file(gdml_file))

    # Remove duplicates
    unique_detectors = []
    seen_detectors = set()
    for det in all_detectors:
        # Use a tuple of name and id to identify unique detectors
        if (det['name'], det['id']) not in seen_detectors:
            unique_detectors.append(det)
            seen_detectors.add((det['name'], det['id']))

    # Sort by Detector ID
    unique_detectors.sort(key=lambda x: int(x['id']))

    # Create directory if it doesn't exist
    output_dir = 'doc/detid'
    os.makedirs(output_dir, exist_ok=True)

    # Write to JSON file
    output_path = os.path.join(output_dir, 'detector_ids.json')
    with open(output_path, 'w') as f:
        json.dump(unique_detectors, f, indent=4)

    print(f"{output_path} created successfully.")
