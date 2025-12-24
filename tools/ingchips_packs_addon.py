import sys
import os
import shutil
import ctypes
import argparse
import platform
import xml.etree.ElementTree as ET
import json
import getpass
import subprocess
import zipfile

LOGGING_ENABLED = True

def log(message):
    if LOGGING_ENABLED:
        print(message)

def zip_folder(folder_path, output_filename):
    with zipfile.ZipFile(output_filename, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for root, dirs, files in os.walk(folder_path):
            for file in files:
                file_path = os.path.join(root, file)
                arcname = os.path.relpath(file_path, folder_path)
                zipf.write(file_path, arcname)

def zping_folder(path):
    folder_to_zip = os.path.join(path, 'flash','KeilFlash')
    pack_folder = os.path.join(folder_to_zip, 'pack')
    zip_filename = os.path.join(folder_to_zip, '0.9.0.zip')
    pack_filename = os.path.join(folder_to_zip, '0.9.0.pack')

    log(folder_to_zip)
    log(pack_folder)

    if not os.path.exists(pack_folder):
        log(f"directory {pack_folder} not found!")
        return
    
    if os.path.exists(zip_filename):
        try:
            os.remove(zip_filename)
            log(f"Removed existing zip file: {zip_filename}")
        except Exception as e:
            log(f"Failed to remove zip file {zip_filename}: {str(e)}")
            return
    
    if os.path.exists(pack_filename):
        try:
            os.remove(pack_filename)
            log(f"Removed existing pack file: {pack_filename}")
        except Exception as e:
            log(f"Failed to remove pack file {pack_filename}: {str(e)}")
            return

    try:
        zip_folder(pack_folder, zip_filename)
        log(f"directory {pack_folder} zipped to: {zip_filename}")
    except Exception as e:
        log(f"Failed to create zip file: {str(e)}")
        return

    try:
        os.rename(zip_filename, pack_filename)
        log(f"zipped file renamed to: {pack_filename}")
    except Exception as e:
        log(f"Failed to rename zip to pack: {str(e)}")
        if os.path.exists(zip_filename):
            os.remove(zip_filename)
            log(f"Cleaned up zip file: {zip_filename}")
        return

def is_admin():
    try:
        return ctypes.windll.shell32.IsUserAnAdmin()
    except:
        return False 
    
def create_dirs(*paths):
    for path in paths:
        try:
            os.makedirs(path, exist_ok=True)
            log(f"created directory: {path}")
        except Exception as e:
            log(f"failed to create directory: {str(e)}")
            sys.exit(1)

def elevate_admin(target, sdk_path,jlink_path):

    if os.name == 'nt':
        params = f'"{__file__}" -t {target} -tp "{sdk_path}" -p "{jlink_path}"'
        ctypes.windll.shell32.ShellExecuteW(None, "runas", sys.executable, params, None, 1)
    else:
        subprocess.run(['sudo', sys.executable, __file__, '--target', target, '--path', jlink_path, '--tools-path', tools_path, '--admin'])

def merge_xml(source, target):
    try:
        src_tree = ET.parse(source)
        dst_tree = ET.parse(target) if os.path.exists(target) else ET.ElementTree(ET.Element("Database"))
        src_root = src_tree.getroot()
        dst_root = dst_tree.getroot()

        existing_devices = set()
        for device in dst_root.findall("Device"):
            chip_info = device.find("ChipInfo")
            if chip_info is not None:
                name = chip_info.get("Name")
                if name:
                    existing_devices.add(name)

        for device in src_root.findall("Device"):
            chip_info = device.find("ChipInfo")
            if chip_info is not None:
                name = chip_info.get("Name")
                if name and name not in existing_devices:
                    dst_root.append(device)
                    log(f"add Device: {name}")
                else:
                    log(f"Skip Existing Device: {name}")
        dst_tree.write(target, encoding="utf-8", xml_declaration=False)
        log(f"Updated XML file: {target}")
        return True
    except Exception as e:
        log(f"XML merge failure: {str(e)}")
        return False  # 

def perform_replace(jlink_path,tools_path):

    sdk_path = tools_path
    
    if not all(os.path.exists(p) for p in [tools_path, jlink_path]):
        log(f"Path error: SDK or J-Link directory does not exist")
        return False

    try:
        sdk_xml = os.path.join(sdk_path, "flash", "SEGFlash", "JLinkDevices.xml")
        jlink_xml = os.path.join(jlink_path, "JLinkDevices.xml")
        
        if not os.path.exists(sdk_xml):
            log("JLinkDevices.xml is missing from the SDK.")
            return False

        if os.path.exists(jlink_xml):
            if not merge_xml(sdk_xml, jlink_xml):
                return False
        else:
            shutil.copy(sdk_xml, jlink_xml)

        src_algo = os.path.join(sdk_path, "flash", "SEGFlash", "Ingchips")
        dst_algo = os.path.join(jlink_path, "Devices", "Ingchips")
        
        if os.path.exists(dst_algo):
            shutil.rmtree(dst_algo)
        shutil.copytree(src_algo, dst_algo)
        log(f"Algorithm packages have been updated to: {dst_algo}")
        return True

    except Exception as e:
        log(f"failure of an operation: {str(e)}")
        return False  

def perform_replace_v7(tools_path):

    sdk_path = os.path.join(tools_path, "flash")
    
    if not all(os.path.exists(p) for p in [tools_path,]):
        log(f"Path error: SDK directory does not exist")
        return False
    if platform.system() == "Windows":
        username = os.getenv("USERNAME")
        user_segger = os.path.join(f"C:\\Users\\{username}", "AppData", "Roaming", "SEGGER")
    elif platform.system() == "Linux":
        user_segger = os.path.join(os.path.expanduser("~"), ".config", "SEGGER")
    else:
        log("Unsupported platform")
        sys.exit(1)

    if not os.path.exists(user_segger):
        os.makedirs(user_segger)
        
    jlink_devices = os.path.join(user_segger, "JLinkDevices")
    devices_dir = os.path.join(jlink_devices, "Devices")
    create_dirs(jlink_devices, devices_dir)

    src_xml = os.path.join(sdk_path, "SEGFlash", "JLinkDevices.xml")
    dst_xml = os.path.join(jlink_devices, "JLinkDevices.xml")
    
    if os.path.exists(src_xml):
        if not os.path.exists(dst_xml):
            shutil.copy(src_xml, dst_xml)
            log("Copied JLinkDevices.xml to user directory")
        else:
            merge_xml(src_xml, dst_xml)

    src_algo = os.path.join(sdk_path, "SEGFlash", "Ingchips")
    dst_algo = os.path.join(devices_dir, "Ingchips")
    
    try:
        if os.path.exists(dst_algo):
            shutil.rmtree(dst_algo)
        shutil.copytree(src_algo, dst_algo)
        log(f"Copied algorithm packages to user directory: {dst_algo}")
    except Exception as e:
        log(f"File copy failure: {str(e)}")
        sys.exit(1)

    log("\nFinal directory structure:")
    for root, dirs, files in os.walk(jlink_devices):
        level = root.replace(jlink_devices, '').count(os.sep)
        indent = ' ' * 4 * level
        log(f"{indent}{os.path.basename(root)}/")
        subindent = ' ' * 4 * (level + 1)
        for f in files:
            log(f"{subindent}{f}")

def pyocd_update(tools_path):

    try:
        username = getpass.getuser()
        
        if platform.system() == "Windows":
            target_base = os.path.join(r"C:\Users", username, r"AppData\Local\cmsis-pack-manager\cmsis-pack-manager")
        elif platform.system() == "Linux":
            target_base = os.path.join(r"/home", username, r".local/share/cmsis-pack-manager/cmsis-pack-manager")
        else:
            log(f"Unsupported platform: {platform.system()}")

        source_pack_dir = os.path.join(tools_path, "flash", "KeilFlash")

        if not os.path.exists(target_base):
            log(f"Error: Target directory does not exist\n{target_base}\nPlease check if pyocd is installed and run 'pyocd pack update'")
            return

        ingchips_dir = os.path.join(target_base, "INGChips")
        ingchips_dfp_dir = os.path.join(ingchips_dir, "ING9_DeviceFamilyPack")

        if os.path.exists(ingchips_dfp_dir):
            shutil.rmtree(ingchips_dfp_dir)
            log(f"Removed existing directory: {ingchips_dfp_dir}")
        os.makedirs(ingchips_dfp_dir, exist_ok=True)

        src_pack = os.path.join(source_pack_dir, "0.9.0.pack")
        dst_pack = os.path.join(ingchips_dfp_dir, "0.9.0.pack")
        
        if not os.path.exists(src_pack):
            log(f"fatal: source .pack file does not exist {src_pack}")
            return
        
        if os.path.exists(dst_pack):
            os.remove(dst_pack)
            log(f"Removed existing pack file: {dst_pack}")
            
        shutil.copy(src_pack, dst_pack)
        os.remove(src_pack)
        log(f"Copied .pack file to {dst_pack}")

        source_index = os.path.join(source_pack_dir, "index.json")
        if not os.path.exists(source_index):
            log(f"fatal: source index.json file does not exist {source_index}")
            return

        with open(source_index, 'r', encoding='utf-8') as f:
            try:
                source_data = json.load(f)
            except json.JSONDecodeError:
                log("fatal: source index.json format is invalid")
                return

        chips_to_sync = list(source_data.keys())
        if not chips_to_sync:
            log("warning: source file does not contain valid chip configurations")
            return

        target_index = os.path.join(target_base, "index.json")
        updated = False
        
        if os.path.exists(target_index):
            with open(target_index, 'r+', encoding='utf-8') as f:
                try:
                    target_data = json.load(f)
                except json.JSONDecodeError:
                    log("fatal: target index.json format is invalid")
                    return
                for chip_id in chips_to_sync:
                    if chip_id not in target_data:
                        target_data[chip_id] = source_data[chip_id]
                        log(f"added chip: {chip_id}")
                        updated = True

                if updated:
                    f.seek(0)
                    json.dump(target_data, f, indent=2, ensure_ascii=False)
                    f.truncate()
        else:
            with open(target_index, 'w', encoding='utf-8') as f:
                json.dump(source_data, f, indent=2, ensure_ascii=False)
                updated = True
                log("Created new index.json file")
            
        if not updated:
            log("All chip configurations are up-to-date, no need to update")
    except Exception as e:
        log(f"Operation failed: {str(e)}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Update J-Link and PyOCD algorithm packages.')
    parser.add_argument('--target', '-t', required=True, help='Type of target. Example: "jlink_v6" "jlink_v7" or "pyocd"')
    parser.add_argument('--path', '-p', required=False, help='Path to J-Link installation (required for jlink_v6) like "C:\Program Files\SEGGER\JLink"')
    parser.add_argument('--tools-path', '-tp', required=True, help='Path to tools installation (required for some targets)')
    args = parser.parse_args()

    target_types = ['jlink_v6', 'jlink_v7', 'pyocd']

    if args.target == 'jlink_v6' and not is_admin():
        print("Requesting administrator privileges....")
        elevate_admin(args.target, args.tools_path, args.path)
        sys.exit()
    if args.target == 'jlink_v6' and not args.path:
        print("Error: --path is required when --target is jlink_v6.")
        sys.exit(1)
    if args.target not in target_types:
        print(f"Error: --target must be one of {target_types}")

    if args.target == 'jlink_v6':
        if perform_replace(args.path,args.tools_path):
            print("The operation was completed successfully!")
            input("Press Enter to exit...")
        else:
            print("Operation not completed, please check the error message.")
            sys.exit(1)
    elif args.target == 'jlink_v7':
        perform_replace_v7(args.tools_path)
        input("Press Enter to exit...")
    elif args.target == 'pyocd':
        zping_folder(args.tools_path)
        pyocd_update(args.tools_path)
        print("PyOCD update is not supported yet.")
    else:
        print(f"Error: --target must be one of {target_types}")
