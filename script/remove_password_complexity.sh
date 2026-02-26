#!/bin/bash

# Check current policy
function check_policy() {
    echo "Current PAM password policy:"
    grep -E 'pam_pwquality|pam_cracklib' /etc/pam.d/common-password || echo "No pam_pwquality or pam_cracklib configuration found"
    echo "\nCurrent /etc/security/pwquality.conf content:"
    cat /etc/security/pwquality.conf 2>/dev/null || echo "No /etc/security/pwquality.conf file found"
}

# Backup configuration files
function backup_files() {
    echo "Backing up configuration files..."
    cp /etc/pam.d/common-password /etc/pam.d/common-password.bak
    [ -f /etc/security/pwquality.conf ] && cp /etc/security/pwquality.conf /etc/security/pwquality.conf.bak
    echo "Backup completed."
}

# Modify password complexity policy
function modify_policy() {
    echo "Modifying PAM password policy..."
    sed -i.bak -E 's/^(password\s+requisite\s+pam_pwquality\.so).*/\1 retry=3 minlen=1 difok=0 ucredit=0 lcredit=0 dcredit=0 ocredit=0/' /etc/pam.d/common-password 2>/dev/null || \
    sed -i.bak -E 's/^(password\s+requisite\s+pam_cracklib\.so).*/\1 retry=3 minlen=1 difok=0 ucredit=0 lcredit=0 dcredit=0 ocredit=0/' /etc/pam.d/common-password

    if [ -f /etc/security/pwquality.conf ]; then
        sed -i.bak -E 's/^\s*minlen.*/minlen = 1/' /etc/security/pwquality.conf
        sed -i -E 's/^\s*minclass.*/minclass = 0/' /etc/security/pwquality.conf
        cat >> /etc/security/pwquality.conf <<'EOF'
dcredit = 0
ucredit = 0
lcredit = 0
ocredit = 0
difok = 0
EOF
    fi
    echo "Modification completed."
}

# Test the changes
function test_policy() {
    echo "Testing the changes. Please try setting a simple password:"
    passwd
}

# Rollback changes
function rollback_policy() {
    echo "Rolling back changes..."
    [ -f /etc/pam.d/common-password.bak ] && cp /etc/pam.d/common-password.bak /etc/pam.d/common-password
    [ -f /etc/security/pwquality.conf.bak ] && cp /etc/security/pwquality.conf.bak /etc/security/pwquality.conf
    echo "Rollback completed."
}

# Main menu
function main() {
    echo "Select an option:"
    echo "1. Check current policy"
    echo "2. Backup configuration files"
    echo "3. Modify password complexity policy"
    echo "4. Test the changes"
    echo "5. Rollback changes"
    echo "6. Exit"
    read -p "Enter your choice: " choice

    case $choice in
        1) check_policy ;;
        2) backup_files ;;
        3) modify_policy ;;
        4) test_policy ;;
        5) rollback_policy ;;
        6) exit 0 ;;
        *) echo "Invalid option" ;;
    esac
}

# Ensure the script is run as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run this script as root."
    exit 1
fi

# Loop the main menu
while true; do
    main
done