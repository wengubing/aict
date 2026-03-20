#!/usr/bin/env python3
import os
import shutil
import argparse
import sys

def select_directory(title="Select Directory"):
    """Open directory selection dialog using tkinter"""
    try:
        import tkinter as tk
        from tkinter import filedialog
        
        # Create a hidden root window
        root = tk.Tk()
        root.withdraw()
        root.attributes('-topmost', True)
        
        # Open directory selection dialog
        directory = filedialog.askdirectory(title=title)
        
        # Destroy the root window
        root.destroy()
        
        return directory.strip() if directory else None
    except ImportError:
        print("Error: tkinter not available. Please install tkinter or use manual input.")
        return None
    except Exception as e:
        print(f"Error opening directory dialog: {str(e)}")
        return None

def find_pdf_files(source_dir):
    pdf_files = []
    for root, dirs, files in os.walk(source_dir):
        for file in files:
            if file.lower().endswith('.pdf'):
                pdf_files.append(os.path.join(root, file))
    return pdf_files

def copy_pdf_files(pdf_files, dest_dir):
    os.makedirs(dest_dir, exist_ok=True)
    
    copied_count = 0
    for pdf_file in pdf_files:
        try:
            file_name = os.path.basename(pdf_file)
            dest_path = os.path.join(dest_dir, file_name)
            
            if os.path.exists(dest_path):
                print(f"Skipping existing file: {file_name}")
                continue
            
            shutil.copy2(pdf_file, dest_path)
            print(f"Copied: {pdf_file} -> {dest_path}")
            copied_count += 1
        except Exception as e:
            print(f"Failed to copy: {pdf_file} - {str(e)}")
    
    return copied_count

def main():
    parser = argparse.ArgumentParser(description='PDF Copy Tool - Recursively copy PDF files to target directory')
    parser.add_argument('-s', '--source', help='Source directory path', required=False)
    parser.add_argument('-d', '--dest', help='Destination directory path', required=False)
    parser.add_argument('-y', '--yes', action='store_true', help='Skip confirmation prompt')
    args = parser.parse_args()
    
    if args.source and args.dest:
        source_dir = args.source
        dest_dir = args.dest
    else:
        # Offer choice between dialog and manual input
        print("Choose input method:")
        print("1. Use Windows Explorer to select directory (Recommended)")
        print("2. Enter directory path manually")
        
        choice = input("Enter your choice (1/2): ").strip()
        
        if choice == '1':
            print("Opening directory selection dialog...")
            source_dir = select_directory("Select Source Directory")
            if not source_dir:
                print("Error: No source directory selected")
                return
            
            dest_dir = select_directory("Select Destination Directory")
            if not dest_dir:
                print("Error: No destination directory selected")
                return
        elif choice == '2':
            source_dir = input("Source directory: ").strip()
            dest_dir = input("Destination directory: ").strip()
        else:
            print("Invalid choice. Using manual input.")
            source_dir = input("Source directory: ").strip()
            dest_dir = input("Destination directory: ").strip()
    
    if not os.path.exists(source_dir):
        print(f"Error: Source directory '{source_dir}' does not exist")
        return
    
    print(f"Searching for PDF files in '{source_dir}'...")
    pdf_files = find_pdf_files(source_dir)
    
    if not pdf_files:
        print("No PDF files found")
        return
    
    print(f"\nFound {len(pdf_files)} PDF files:")
    for pdf_file in pdf_files:
        print(f"  - {pdf_file}")
    
    if not args.yes:
        confirm = input(f"\nCopy to '{dest_dir}'? (y/n): ").strip().lower()
        if confirm != 'y':
            print("Operation cancelled")
            return
    
    print(f"\nCopying files to '{dest_dir}'...")
    copied_count = copy_pdf_files(pdf_files, dest_dir)
    
    print(f"\nCopy completed! {copied_count} files copied")

if __name__ == "__main__":
    main()