---
date: "2018-09-15T17:50:37Z"
title: Smart India Hackathon 2018
---
## Smart India Hackathon 2018

In January this year, I along with my friends got an opportunity to participate in the Smart India Hackathon 2018. In this blog post, I want to share my experience of attending this hackathon and the lessons we learnt.

Smart India Hackathon is a 36-hour hackathon organized by the government of India. It is an opportunity for students to use their innovative ideas to solve real-world problems. The Indian government started conducting this hackathon from 2017 and this is the second hackathon. 

Students from all Indian colleges can participate in this hackathon. The students must form teams of 6 students and optionally 2 mentors. The mentors can be teachers or experts from the industry.

We were a team of 6 students: Me, Akash, Aswin, Allwin, Aravindhan and Ajith. We named our team Alpha6.

![The Team](../../../assets/smartindiahackathon2018/one.jpeg "The Team")

### Idea submission
In December 2017, the application period for the Smart India Hackathon had started. The list of problem statements from different ministries was uploaded to the hackathon website. Some of the participating ministries were the ministry of farmer's welfare, the ministry of information and broadcasting, etc. Different government organizations like ISRO, DRDO, etc also uploaded their problem statements online. 

The students can read these problem statements and propose a solution to that problem. The teams had to describe their solution using presentation slides and submit it. We applied for 6 different problem statements which we found interesting.

On March 1st, The selected teams were announced. We were excited to see our names on the list. We were selected to work on Real-time identification of crops, plant diseases and weeds under the Ministry of Agriculture and Farmer's Welfare.

Every team was also assigned a nodal centre based on the ministry or organization under which they were selected. The teams had to attend the hackathon at these nodal centres. Our nodal centre was in Chandigarh.

### Preparation

After we got selected, we had one month to prepare. During this month, we learnt the basics of Python, machine learning and android app development. 

Our problem statement needed image recognition so we had to learn machine learning. Python has become the language of choice for creating and training machine learning models. We had to learn Python too.
Python is an easy language to learn. We found tons of tutorials online. We started using Jupyter notebooks to quickly prototype and test code.

For the machine learning part, we read a lot of blogs and tutorials. For image recognition, we had to use Convolutional Neural Networks.

### Project Spot

Our idea was to create an android app that allows farmers to identify plant diseases just by taking a picture of the plant's leaf. The android app will communicate with a backend server where trained models will use the picture to predict the type of plant and the disease.

For our backend, we planned to use Python Flask to create a basic REST API and Keras framework to build and train the model. The android app will interact with the server using the API.

One of our biggest challenges was collecting the dataset. We needed labelled images of plant leaves with diseases to train the model. We collected images for 3 plants and 3 diseases for each plant. There were not many sources we could find. We downloaded some images from [ipmimages](https://ipmimages.org). The dataset was too small so we downloaded leaf images using image search and then deleted images that were irrelevant.

### Hackathon day

![Time to code](../../../assets/smartindiahackathon2018/two.jpeg)

On the hackathon day, we reached the nodal centre, CGC college at Landran. We ate breakfast and assembled at the hackathon venue. Everyone was so excited. Once we reached our table we set up our standee and took out our laptops.
The event started with the speeches by a number of dignitaries.
And then the timer was set and the coding period started. 

We split ourselves into two groups. Aswin, Akash and Ajith worked on the Android app. Aravindhan, Allwin and I, we worked on the backend. We discussed the API endpoints that the backend has to provide for the app to communicate with the backend.

So the basic structure of our app was this. The flask backend will provide a REST API and receive HTTP requests. The android app will send an image of the plant to the server. Once the image is received at the server, It will be run on the trained models to get a prediction. Then this prediction will be sent to the android app which will show the results to the user.

It took a lot of time to train the models using the CPU. Allwin and Aravindhan installed Cuda toolkit in one of our laptops. This reduced our training time a lot. We just wish we had set up Cuda before we went there.

We also localized our app to support different languages. To demonstrate this we made the app in English and Tamil.

The volunteers from Chandigarh engineering college were very friendly. We were never hungry because they kept on bringing snacks and water bottles to every table. They played loud music to keep us awake during the night and early in the morning, we did some exercise and yoga outdoors.

Presenting to the judges is the crucial part. We had 6 mentoring sessions: 3 training sessions and 3 judging sessions. The people usually come in groups consisting of both technical people and people from the ministry. Technical people were people from the IT industry who were interested in the technologies used and implementation of the app. The people from the ministry were experts in their domain and they seemed to be more interested in how useful the app will be to the end user, in our case, farmers.

![Team at work](../../../assets/smartindiahackathon2018/three.jpeg)

We used [ngrok](https://ngrok.io) to tunnel the requests from the Android app to the Python server. Ngrok is a service that allows creating tunnels to certain ports on our local computer. It provides us with a URL which can be used to send requests to the local computer from anywhere over the Internet. The problem with the free version of Ngrok is that whenever we restart Ngrok we get assigned a new URL. We had to hardcode the URL in the Android app so we had to keep the service running throughout the hackathon period to prevent the URL from changing. 

But after the coding period was over, I shut down my laptop which was running the server and the Ngrok service. Because of this, we couldn't give the judges a demo during the last review. We showed the judging panel screencasts of the app that we had shot earlier.

### Lessons learnt

* Participate in hackathons whenever possible. It is a totally different experience and you will learn a lot. Also learning with friends is always fun.

* Before any hackathon, talk to the potential users of the application you are going to create. During one of the reviews, a member of the judging panel asked us if we had talked to farmers about the application.

* Also if your project is related to Machine learning, make sure you prepare a very good dataset. We did all we could to get a dataset before we went to the hackathon but still, our model's accuracy was just average. 

* Talk with other people. We regret not doing this. After all, a hackathon is not just about writing code. It is an opportunity to socialize.

This was our first hackathon. We didn't win but the experience itself was something special. It is a very good initiative by the Indian government. It is a step towards inculcating the hacker culture among the students. If you are studying in college, then you should definitely give this a try.
