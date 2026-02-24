#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Honor X80i Release Tracker
Features:
1. Daily automatic web scraping to monitor Honor X80i release plans
2. Optional free AI model integration for analysis and prediction
3. Notification push functionality
4. Scheduled task support
"""

import requests
from bs4 import BeautifulSoup
import time
import schedule
import json
import os
from datetime import datetime

CONFIG_FILE = 'config.json'

class HonorX80iTracker:
    def __init__(self):
        self.config = self.load_config()
        self.last_status = self.load_last_status()
        
    def load_config(self):
        """Load configuration file"""
        default_config = {
            "notification": {
                "email": {
                    "enable": False,
                    "sender": "your_email@example.com",
                    "password": "your_password",
                    "receiver": "your_receiver@example.com",
                    "smtp_server": "smtp.example.com",
                    "smtp_port": 587
                }
            },
            "sources": {
                "honor_official": "https://www.honor.com/cn/",
                "tech_news": [
                    "https://www.ithome.com/",
                    "https://www.pconline.com.cn/"
                ]
            },
            "ai": {
                "enable": False,
                "api_key": "your_api_key",
                "api_url": "https://api.example.com/v1/chat/completions"
            }
        }
        
        if os.path.exists(CONFIG_FILE):
            with open(CONFIG_FILE, 'r', encoding='utf-8') as f:
                return json.load(f)
        else:
            with open(CONFIG_FILE, 'w', encoding='utf-8') as f:
                json.dump(default_config, f, indent=4, ensure_ascii=False)
            return default_config
    
    def load_last_status(self):
        """Load last run status"""
        status_file = 'last_status.json'
        if os.path.exists(status_file):
            with open(status_file, 'r', encoding='utf-8') as f:
                return json.load(f)
        else:
            return {
                "last_check_time": None,
                "last_status": "No release information found",
                "release_time_confirmed": False,
                "release_time": None
            }
    
    def save_last_status(self):
        """Save current status"""
        status_file = 'last_status.json'
        with open(status_file, 'w', encoding='utf-8') as f:
            json.dump(self.last_status, f, indent=4, ensure_ascii=False)
    
    def fetch_honor_official(self):
        """Fetch Honor official website information"""
        try:
            url = self.config['sources']['honor_official']
            headers = {
                'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
            }
            response = requests.get(url, headers=headers, timeout=10)
            response.encoding = response.apparent_encoding
            
            soup = BeautifulSoup(response.text, 'html.parser')
            content = soup.get_text().lower()
            
            keywords = ['honor x80i', 'release', 'launch']
            matches = []
            for keyword in keywords:
                if keyword in content:
                    matches.append(keyword)
            
            return {
                'source': 'Honor Official',
                'url': url,
                'has_info': len(matches) > 0,
                'matches': matches,
                'content': content[:500] + '...'
            }
        except Exception as e:
            return {
                'source': 'Honor Official',
                'url': url,
                'has_info': False,
                'error': str(e)
            }
    
    def fetch_tech_news(self):
        """Fetch tech news website information"""
        results = []
        for url in self.config['sources']['tech_news']:
            try:
                headers = {
                    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
                }
                response = requests.get(url, headers=headers, timeout=10)
                response.encoding = response.apparent_encoding
                
                soup = BeautifulSoup(response.text, 'html.parser')
                content = soup.get_text().lower()
                
                keywords = ['honor x80i', 'release', 'launch']
                matches = []
                for keyword in keywords:
                    if keyword in content:
                        matches.append(keyword)
                
                results.append({
                    'source': url.split('.')[1],
                    'url': url,
                    'has_info': len(matches) > 0,
                    'matches': matches,
                    'content': content[:500] + '...'
                })
            except Exception as e:
                results.append({
                    'source': url.split('.')[1],
                    'url': url,
                    'has_info': False,
                    'error': str(e)
                })
        return results
    
    def analyze_with_ai(self, data):
        """Analyze release information with AI model"""
        if not self.config['ai']['enable']:
            return "AI analysis disabled"
        
        try:
            api_url = self.config['ai']['api_url']
            api_key = self.config['ai']['api_key']
            
            prompt = f"Please analyze the following information about Honor X80i phone, extract release plans and possible release time: {json.dumps(data, ensure_ascii=False)}"
            
            headers = {
                'Content-Type': 'application/json',
                'Authorization': f'Bearer {api_key}'
            }
            
            payload = {
                "model": "gpt-3.5-turbo",
                "messages": [
                    {"role": "system", "content": "You are a professional tech news analyst, good at extracting and predicting phone release information."},
                    {"role": "user", "content": prompt}
                ],
                "max_tokens": 500
            }
            
            response = requests.post(api_url, headers=headers, json=payload, timeout=30)
            response.raise_for_status()
            
            return response.json()['choices'][0]['message']['content']
        except Exception as e:
            return f"AI analysis failed: {str(e)}"
    
    def send_notification(self, title, message):
        """Send notification"""
        print(f"\n=== Notification ===")
        print(f"Title: {title}")
        print(f"Content: {message}")
        print("================")
    
    def check_release_info(self):
        """Check Honor X80i release information"""
        print(f"\n[{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}] Starting to check Honor X80i release information...")
        
        official_info = self.fetch_honor_official()
        print(f"Honor Official: {'Has related information' if official_info['has_info'] else 'No related information'}")
        
        news_info = self.fetch_tech_news()
        for news in news_info:
            print(f"{news['source']}: {'Has related information' if news['has_info'] else 'No related information'}")
        
        all_info = {
            'official': official_info,
            'tech_news': news_info,
            'check_time': datetime.now().isoformat()
        }
        
        ai_analysis = self.analyze_with_ai(all_info)
        print(f"AI Analysis Result: {ai_analysis}")
        
        has_new_info = False
        new_info_content = ""
        
        if official_info['has_info']:
            has_new_info = True
            new_info_content += f"Honor Official found related information: {official_info['content']}\n\n"
        
        for news in news_info:
            if news['has_info']:
                has_new_info = True
                new_info_content += f"{news['source']} found related information: {news['content']}\n\n"
        
        self.last_status['last_check_time'] = datetime.now().isoformat()
        
        if has_new_info:
            keywords = ['confirmed', 'announced', 'official', 'release time']
            has_confirmed_time = any(keyword in new_info_content for keyword in keywords)
            
            if has_confirmed_time:
                self.last_status['release_time_confirmed'] = True
                self.last_status['release_time'] = datetime.now().isoformat()
                self.send_notification("[URGENT] Honor X80i release time confirmed!", new_info_content + f"\nAI Analysis: {ai_analysis}")
            else:
                self.send_notification("[UPDATE] New progress on Honor X80i release plan", new_info_content + f"\nAI Analysis: {ai_analysis}")
            
            self.last_status['last_status'] = new_info_content[:200] + "..."
        else:
            self.last_status['last_status'] = "No new release information"
            print("No new release information found")
        
        self.save_last_status()
        print(f"[{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}] Check completed")
    
    def run_daily_check(self):
        """Run daily check"""
        self.check_release_info()
    
    def start_scheduler(self):
        """Start scheduled task"""
        schedule.every().day.at("10:00").do(self.run_daily_check)
        
        print("Honor X80i Release Tracker started")
        print("Automatically checks release information every day at 10:00 AM")
        print("Press Ctrl+C to exit")
        
        self.check_release_info()
        
        while True:
            schedule.run_pending()
            time.sleep(60)

def main():
    """Main function"""
    tracker = HonorX80iTracker()
    tracker.start_scheduler()

if __name__ == "__main__":
    main()
