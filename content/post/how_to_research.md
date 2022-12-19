+++
title = "How to do research?"
date = 2022-12-06
tags = ["research"]
draft = false
+++

Disclaimer: I'm no expert in this. This post is just to collect all my thoughts and lessons learnt from random talks and blogs, about research.

**What is research?**

Research is producing new knowledge.
The aim of research is to do something **novel** (new) and **useful**.
The purpose of literature survey is to ensure that whatever idea we come up with is new and has not been proposed before.
And the purpose of evaluations is to show that our idea or technique is useful.

**How to do research?**

The research process: I like to think of the process as a pipeline. It's kind of like a computer. We have input, processing and output.

1.  **Input**: reading about ideas (in papers, courses or blogs) and learning new skills. (Skills are about _how to_ do something, while ideas are about _what_)
2.  **Processing**: critiquing ideas, generating new ideas and evaluating them. This is where the real knowledge generation happens.
3.  **Output**: writing about your ideas (in papers mainly), presenting your ideas to other researchers and discussing them.


## Input {#input}

The primary source of ideas and information is research papers.
We can also get it from other documents, blogs and conference presentations.

Some of the general skills you will need as a researcher are

1.  Touch typing
2.  LaTeX: for typesetting all your reports, papers, slides, etc. LaTeX (and bibtex) allows us to forget about formatting, citation styles, etc and focus on the content. Most conferences provide LaTeX templates for their papers that follows the conference formatting guidelines. If you use their template, you don't have to worry about the conference's formatting guidelines at all.
3.  Writing: You will be spending a lot of time writing about things. So it's better to get good at it.


## Processing {#processing}


### Coming up with new ideas to work on {#coming-up-with-new-ideas-to-work-on}

This is the crux of research. How do we come up with something new?
There are a few strategies.

1.  Classify existing works and identify gaps (the table method):

    This was suggested to me by Prof. Rupesh Nasre. It involves creating a table, where columns and rows represent different dimensions of the problem.
    Now we classify all the existing works in the area based on these two (or more) dimensions.
    Finally, identify boxes where there are not much work done and explore those areas.
2.  Identify all possible techniques that could be used to solve a problem
3.  Borrow ideas from a different subject or context

    Examples of such works that I've come across are:

    -   _Theory and Practice of Finding Eviction Sets_ by Vila, et. al:
        _Threshold group testing_ is a concept from statistics which is used in testing a group of blood samples with
        lesser number of tests. This works propose a technique for finding minimal eviction sets for microarchitectural attacks by using threshold group testing.

4.  If there are two main techniques to do something, propose a hybrid of the two.

      Often, there are some problems where there are two main, opposing techniques that
    make different tradeoffs. One option to explore in such problems, is to combine and use both techniques together.
      Examples of such works that I've come across are:

    -   _Agile Paging: Exceeding the Best of Nested and Shadow Paging_ by Gandhi, et. al:
        This work combines the two main techniques used for paging in virtualized systems: Nested paging and Shadow paging.
    -   _A tale of two erasure codes in HDFS_ by Xia, et. al:
        This work combines and uses two different erasure codes: a fast code (which is faster but has size overheads)
        and a compact code (which is compact but is slower).

5.  Combine all the individual techniques for solving your problem into a framework that uses all of the techniques together,
     switches between the techniques dynamically, or uses some of the techniques based on heuristics
    Examples of such works that I've come across are:
    -   _A Framework for Memory Oversubscription Management in Graphics Processing Units_ by Li, et. al:
        Combines three different techniques: Pre eviction, Core throttling and Memory compression to reduce
        unified memory overheads during oversubscription.

6.  Create or discover something that the research community can use.
    -   Simulators
    -   Benchmark suites
    -   Performance models
    -   Taxonomy

7.  Apply AI/ML to your (systems) problem 😜. Because why not? And don't forget to mention that you are the first to use AI/ML for that problem.


### Automation {#automation}

When evaluating, it helps to automate as much as possible.
Especially in systems research, where evaluation usually involves combining multiple software tools together to perform something.
We can automate running experiments, collecting logs/output and generating some basic plots out of the data so that you can reason about the results visually.
