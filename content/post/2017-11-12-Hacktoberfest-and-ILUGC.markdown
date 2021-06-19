+++
title = "Hacktoberfest and ILUGC meetup"
date = 2017-11-12T13:28:37Z
tags = ["hacktoberfest", "ILUGC"]
draft = false
+++

![Hacktoberfest](../../../assets/hacktoberfest/hacktoberfest.png "Hactoberfest")

In this post, I want to share what I did for [Hacktoberfest](https://hacktoberfest.digitalocean.com).

Hacktoberfest is a month-long program to motivate people to contribute to open source software. All you have to do is give 4 pull requests to any public repository on Github. The reward is a cool t-shirt and Github stickers. These contributions should be made in the month of October to be counted for hacktoberfest.

I searched for a project I can contribute to. I came across repositories that are made for hacktoberfest. People just add their name to some file and give a pull request. I think such repositories are spoiling the spirit of hacktoberfest. The only good thing is that people who are completely new to git and github can practice using git on these repos.

I came to know of another open source contribution competition called [CodeHeat](https://codeheat.org). It is similar to Hacktoberfest. Both motivate people to become open source contributors. But the similarities end there. CodeHeat happens till next February and has a restriction that the contributions should be made to projects under the organization FOSSASIA. So contributing to CodeHeat will add to Hacktoberfest. I applied for both.

## SUSI.AI

I got interested in a project called [SUSI.AI](https://chat.susi.ai/overview). It is an artificially intelligent personal assistant being developed by the FOSSASIA community. The aim of the project is to create an artificial intelligence, to which adding new skills should be as easy as editing a wiki page. Clients for SUSI.AI is being developed for many platforms like desktop, Android and [the web](https://chat.susi.ai). Some new clients are being built as add-ons for Chrome and Mozilla Firefox.

Having built some basic add-ons for Firefox during MozillaTN meetup 2017 helped me. As soon as I saw the [susi_firefoxbot](https://github.com/fossasia/susi_firefoxbot) repository, I knew I can help out on this one. So I started creating issues and giving pull requests.
Thanks to the welcoming community, I was able to work on a few minor features.

## ILUGC meetup
I had a chance to attend [ILUGC](https://ilugc.in) meetup this month. ILUGC stands for Indian Linux User Group Chennai. ILUGC meetup happens every month on the second Saturday at the IITM campus. I went there with my friend Aravindan. The agenda was to discuss two things.
1. Demonstration of a Tamil text to speech software made by IIT Donlab and SSN college of engineering.
2. Translating street names in OpenStreetMap using a telegram bot.

[T.Shrinivasan](https://goinggnu.wordpress.com/about), a Wikimedian and FOSS evangelist presented the two talks.

The text to speech software was built by people from IIT and SSN college. It was built for major Indic languages. The quality of the audio obtained as output was very nice. The TTS software's source code is not released on any common git platforms like github. Instead, they have a website where the user has to register before accessing the source code. This is because the people at IIT want statistics of the number of downloads.
The text to speech software is written in Perl. It uses a model called Hidden Markov Model, a model very frequently used in speech-related applications.

* [Indic TTS](https://www.iitm.ac.in/donlab/tts/) 
* [Blog post on installation of TTS](https://goinggnu.wordpress.com/2017/09/20/installation-script-for-tamil-text-to-speech-system/) 
* [Installation script](https://github.com/tshrinivasan/tamil-tts-install)

Next, he presented a telegram bot that was built by him and his colleagues. This telegram bot will be used to translate the names of places into Tamil. This data could then be added to OpenStreetMap to provide localized maps in Tamil.
I think translating using chatbots is a brilliant idea.
This could allow people with no programming knowledge to translate strings. I appreciate the team for building this.

* [OSM Tamil bot](https://t.me/osm_tamil_bot) 
* [Source code](https://github.com/Dineshkarthik/OSM-Translate-TelegramBot)

After the meetup, I took a look at the source code of the telegram bot. I didn't know a lot of python, but thanks to the readability of python, I was able to understand the program. I made a few changes to the source code. The changes were small. I fixed a spelling mistake in a word and added some information about other chat commands in the help message.

![Hacktoberfest Stats](../../../assets/hacktoberfest/hacktoberfeststats.png "Hactoberfest Stats")

At the end of the month, I have done 6 pull requests in two projects. All of them got merged.
I learned to use a few important git commands like git rebase, git log and a few more commands. I am also learning to use Inkscape to create logos.
On the whole, I feel like I have spent this month productively.
