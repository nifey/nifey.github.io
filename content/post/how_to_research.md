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
The purpose of literature survey is to ensure that our idea is new and has not been proposed before.
And the purpose of evaluations is to show that our idea or technique is useful.

**How to do research?**

The research process: I like to think of the process as a pipeline. It's kind of like a computer. We have input, processing and output.

1.  **Input**: reading about ideas (in papers, courses or blogs) and learning new skills. (Skills are about _how to_ do something, while ideas are about _what_)
2.  **Processing**: critiquing ideas, generating new ideas and evaluating them. This is where the real knowledge generation happens.
3.  **Output**: writing about your ideas (in papers mainly), presenting your ideas to other researchers and discussing them.


## 1. Input {#1-dot-input}

The primary source of ideas and information is research papers.
We can also get it from other documents, blogs and conference presentations.

Some of the general skills you will need as a researcher are

1.  Touch typing
2.  LaTeX: for typesetting all your reports, papers, slides, etc. LaTeX (and bibtex) allows us to forget about formatting, citation styles, etc and focus on the content. Most conferences provide LaTeX templates for their papers that follows the conference formatting guidelines. If you use their template, you don't have to worry about the conference's formatting guidelines at all.
3.  Writing: You will be spending a lot of time writing about things. So it's better to get good at it.


## 2. Processing {#2-dot-processing}


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

7.  Study anamolies

    During your experiments, if you find some benchmark behaving unexpectedly, dig deeper into it and try to explain what is causing that behaviour.

8.  Study failures

    If it turns out that the idea does not perform well, find out the reason why it doesn't perform well.
    This could give insights for modifying the design to perform better.

9.  Apply AI/ML to your (systems) problem 😜. Because why not? And don't forget to mention that you are the first to use AI/ML for that problem.


### Strong inference {#strong-inference}

-   When you find an interesting observation, come up with multiple hypotheses that can explain the observation and
    perform experiments to falsify the hypotheses, one by one.
    This prevents confirmation bias from attachment to the first hypothesis.


### Evaluation {#evaluation}

This is where we try to answer: Is my idea useful?
The evaluation process is different in different fields, even between subfields.
It's good to spend some time to ensure that the right benchmarks and evaluation methodology is used, before starting with experiments.

-   Choose the right evaluation methodology
    -   When measuring performance on real systems (as opposed to deterministic simulators),
        minimize operating system noise by (i) isolating CPUs using isolcpu boot parameter in Linux
        (ii) Take multiple measurements and use the mean.
    - When assuming evaluation parameters (like simulation paramters), check if they are reasonable, even if they were used in previous papers. Because reviewers will question why the parameters were chosen and if they closely represent real-world parameters.
-   Choose the right metrics
    -   Use the right mean (arithmetic, geometric or harmonic mean) to aggregate your results.
-   Choose the right benchmarks
    - Know your benchmark suites.
    - Get to know what each benchmark does at a high level, and it's behaviour with resepect to your metrics. For instance, if you are trying to reduce virtual memory performance, classify the benchmarks into TLB-friendly and TLB-averse based on TLB hit rates.
    - If there are input data to the benchmark, then check if the choice of input data affects the  performance of the benchmark, in which case, we have to select input data carefully.
- Avoid common benchmarking crimes
    - https://arxiv.org/pdf/1801.02381.pdf
    - http://www.haifux.org/lectures/311/BM_crimes.pdf
    - https://gernot-heiser.org/benchmarking-crimes.html

### Automation {#automation}

When evaluating, it helps to automate as much as possible.
Especially in systems research, where evaluation usually involves combining multiple software tools together to perform something.
We can automate running experiments, collecting logs/output and generating some basic plots out of the data so that you can reason about the results visually.


## 3. Output {#3-dot-output}


### Paper writing {#paper-writing}

-   Motivation
	-   Use a motivating example to convey the challenges and intuitions behind your solution.
-   Evaluation
    -   **Explain anamolies**: If any of your benchmarks perform in a anomolous way, i.e. it performs unexpectedly,
        then give (possible) reasons, preferably backed by other supporting results.
    -   **Explain empirical observations**: If you make any empirical observation, **explain why** with some examples.
        Just giving an observation backed by data is not enough, we need to show some insights into why the observation holds.
    -   **Organize the evaluation section based on research questions** (Learnt from StateFuzz paper):
        In the beginning of the evaluation section, summarize the experiments and list down the Questions that you will be answering in
        the evalution section and also mention in which section they are answered.
-   Writing your first draft
    -   **Imagine nobody is going to read your paper**: If you don't feel like writing, or if you are procrastinating on writing, chances are high that you are worried about what people
        would think when they read your paper.
        This worry is unproductive and we don't need this, atleast not when writing the first draft.
        So, _imagine that nobody is going to read your paper and that you are going to throw it away after you finish writing_.
        This can help keep the words flowing. You can always edit and revise the paper later.
-   Small LaTeX things to remember
    -   When quoting use backticks for beginning quote \`\` ...quoted_text... "
    -   Use &sect; Section symbol (\\​S in LaTeX) instead of repeating section everywhere.
-   Related works section
    The purpose of related works section is (1) to show that you have read the other state-of-the-art techniques used for,
    or related to the problem you are solving and (2) to show that your work is different and novel from the existing literature.
    -   Clearly mention how your technique is different from other closely related works.
    -   Use a table comparing existing works with tick and cross symbols, and show how your work fills the gap better.

{{< youtube 1AYxMbYZQ1Y >}}

### Presenting

- How to give a great research talk
    {{< youtube sT_-owjKIbA>}}
- Poster presentation
    {{< youtube vMSaFUrk-FA>}}

## Artifact submission
- [Artifact Evaluation: Tips for Authors](https://blog.padhye.org/Artifact-Evaluation-Tips-for-Authors/)

## Other resources

- Own your PhD project: How to take charge of your research
    {{< youtube q_rEqcO7hMY>}}
- You and your research
    {{< youtube a1zDuOPkMSw>}}
- [How to have a bad career in Research/Academia](https://people.eecs.berkeley.edu/~pattrsn/talks/BadCareer.pdf) (David Patterson)
- [How to have Real world impact: Five easy steps](https://www.sigarch.org/how-to-have-real-world-impact-five-easy-pieces/)
- [How to do great research](https://greatresearch.org/)
- [DBLP paper search within select conferences](https://sabertazimi.github.io/dblp)
- [Searching paper deadlines of relevant conferences](https://confsearch.ethz.ch/)
