---
title: 'SCONE: Open source software for predicting biological motion'
authors:
- affiliation: 1
  name: Thomas Geijtenbeek
  orcid: 0000-0003-0984-7016
date: "2 April 2019"
bibliography: paper.bib
tags:
- predictive simulation
- neurobiomechanical modeling
- optimization
- data visualization
affiliations:
- index: 1
  name: Delft University of Technology, Delft, The Netherlands
---

# Summary
The study of human and animal movement seeks to understand how the complex interplay between the nervous, muscular and skeletal systems generates physical motion. While the individual components comprising these systems are fairly well understood, the fundamental question of how they function as an integrated whole remains largely unanswered.

The use of predictive simulations with neuromusculoskeletal models has the potential to greatly accelerate this line of research. It allows for detailed analysis of integrated components, and pose true ‘what-if’ questions – allowing to investigate the effect of individual model parameters on the motion as a whole. Even though this promise is widely recognized by the community as the ‘holy grail’ of movement science, progress in this direction has been relatively underwhelming. A main contributing factor is complexity: besides expertise in neurological, musculoskeletal and mechanical modeling, it requires knowledge and understanding of optimization theory, as well as advanced software development skills to tie each of these components together.

SCONE (Simulated Controller OptimizatioN Environment) is an open source software framework that helps researchers to overcome these barriers. It allows researchers to define neuromuscular controllers and optimize their control parameters through simulation, all without programming. It uses a modular approach for both controllers and objectives, allowing researchers to investigate various control strategies for a range of tasks. The software generates optimal motion that produces a task defined through an objective function without the need or constraints imposed by reference motions. SCONE is ‘simulator-agnostic’, it can be used in combination with any dynamics simulation package.

![The SCONE user interface](scone_window.png)

# References
