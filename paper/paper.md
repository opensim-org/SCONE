---
title: 'SCONE: Open source software for predicting biological motion'
authors:
- affiliation: 1
  name: Thomas Geijtenbeek
  orcid: 0000-0003-0984-7016
date: "19 April 2019"
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
Mobility impairments pose an increasing burden on our ageing society, urging researchers and clinicians to invent new assistive technologies and improve medical treatment. However, their efforts are impeded by a gap in our understanding of biological motion. While individual branches of neuroscience and biomechanics have produced a wealth of knownledge on the components comprising biological motion, the fundamental question of how the neural, muscular and skeletal systems operate together to produce efficient and purposeful motion remains largely unanswered.

The use of neurological and musculoskeletal simulation can help improve our understanding of biological motion. Inverse dynamic simulations have been used successfully to estimate quantities of recorded human motion that are not directly observable, such as muscle force or joint torque. Even though these inverse simulations have provided useful insights into human motion, they rely on existing data and cannot predict new behavior.

Predictive forward dynamic simulations do the opposite: they compute motion trajectories that perform a given task optimally, according to high-level objectives such as stability, metabolic energy expenditure and pain avoidance. Predictive simulations enable powerful new applications for musculoskeletal models, such as predicting the outcome of treatment and optimizing the efficiency and efficacy of assistive devices. More fundamentally, it enables researchers to pose true *what-if?* questions, allowing them to investigate the effects of individual model and control parameters on the motion as a whole.

Despite having shown great promise [AnderssonPandy, GeyerHerr, Geijtenbeek], the number of studies that successfully employ predictive forward simulations has been remarkably limited. A main contributing factor is complexity: besides expertise in neurological and musculoskeletal modeling, users require knowledge and understanding of optimization theory, as well as advanced software development skills to tie these components together. Even successful projects often do not permit fruitful collaboration and follow-up research, due to lack of structure and documentation of the resulting code-base.

SCONE (Simulation-based Controller OptimizatioN Environment) is designed to help overcome these obstacles. It is a fully featured software framework that allows researchers to perform, analyze and reproduce custom predictive simulations of biological motion, without the need for software development. Features of SCONE include:
  * A component-based control framework that enables hierarchical composition of neuromuscular control strategies, through use of a simple configuration script.
  * The ability to simultaneously optimize both model and control parameters, according to a user-defined hierarchically composed objective function.
  * SCONE is *simulator-agnostic*, meaning it can be used with any musculoskeletal dynamics simulation software package, after implementing a thin API layer in C++. Support for OpenSim is included in the software.
  * SCONE provides an integrated Graphical User Interface (GUI) that allows simultaneous development, optimization, and analysis of predictive simulations.

![The SCONE user interface](scone_window.png)

More information on SCONE, including tutorials, examples, community guidelines and documentation can be found on https://scone.software.

# References
