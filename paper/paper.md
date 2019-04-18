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
Mobility impairments pose an increasing burden on our ageing society, urging researchers and clinicians to invent new assistive technologies and improve medical treatment. However, their efforts are impeded by a gap in our understanding of biological motion. Even though the individual components comprising the neural, muscular and skeletal systems are well understood, the fundamental question of how these systems work together to produce efficient and effective motion remains largely unanswered.

The use of neurological and musculoskeletal simulation can improve our understanding of biological motion. Inverse musculoskeletal simulations have been used successfully to estimate quantities of recorded human motion that are not directly observable, such as muscle force or joint torque. Even though these inverse simulations have provided useful insights into human motion, they rely on existing data and cannot predict new behavior.

Predictive forward simulations do the opposite: they compute the motion trajectories that perform a given task optimally, according to high-level objectives such as stability, metabolic energy expenditure and pain avoidance. Predictive simulations permit powerful new applications for musculoskeletal models, such as predicting the outcome of treatment and optimizing the efficiency and efficacy of assistive devices. More fundamentally, it enables researchers to pose true *what-if?* questions, allowing them to investigate the effects of individual model parameters on the motion as a whole.

Even though predictive forward simulations have shown great promise, the number of studies employing this approach has been remarkably limited. A main contributing factor is complexity: besides expertise in neurological and musculoskeletal modeling, it requires knowledge and understanding of optimization theory, as well as advanced software development skills to tie these components together. Since most of these researches are project based, the resulting code is not on a level of organization and documentation that allows for fruitful sharing of results.

SCONE (Simulated Controller OptimizatioN Environment) is designed to help alleviate these issues. It is a fully integrated software package that allows researchers to perform custom predictive simulations of biological motion without the need for programming. It features the following components:
  * A component-based control framework that allows for hierarchical composition of neuromuscular control strategies, through use of a simple scripting language.
  * Any musculoskeletal or control parameter can be optimized simultaneously using an derivative-free optimization method, according to a custom hierarchically composed objective function.
  * SCONE is ‘simulator-agnostic’, meaning it can be used in combination with any musculoskeletal dynamics simulation software package – after implementing a thin API layer.
  * SCONE provides an integrated GUI that allows simultaneous development, optimization, and analysis of predictive simulations.

More information on SCONE, including tutorials, examples, community guidelines and documentation can be found on https://scone.software.

![The SCONE user interface](scone_window.png)

# References
