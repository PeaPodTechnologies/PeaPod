# PeaPod <!-- omit in toc -->

[![issuesopen](https://img.shields.io/github/issues/PeaPodTechnologies/peapod)](https://github.com/PeaPodTechnologies/PeaPod/issues) [![issuesclosed](https://img.shields.io/github/issues-closed/PeaPodTechnologies/peapod)](https://github.com/PeaPodTechnologies/PeaPod/issues?q=is%3Aissue+is%3Aclosed) [![opensource](https://img.shields.io/badge/open-source-red)](https://github.com/PeaPodTechnologies/PeaPod/issues?q=is%3Aopen+is%3Aissue+label%3A%22Status%3A+Open%22) ![coffee](https://img.shields.io/badge/powered%20by-coffee-brown) [![24/7](https://img.shields.io/badge/Eat,%20Sleep,-PeaPod-darkgreen)](https://www.youtube.com/watch?v=2zWv9JC5G3w) [![FLDSMDFR](https://img.shields.io/badge/The-FLDSMDFR-orange)](https://www.youtube.com/watch?v=k8xFbWLUDoQ)


A cloud-connected isolated and automated plant growth environment, able to generate any environment from a combination of independent environment parameters. 

Designed as both a hassle-free food production system and a research tool for precise and distributed mapping of the plant-environment relationship.

<!-- TODO: Build statuses? etc. -->

Submitted to the [NASA/CSA Deep Space Food Challenge](https://impact.canada.ca/en/challenges/deep-space-food-challenge).

<a href="https://www.youtube.com/watch?v=2zWv9JC5G3w" target="_blank"><img src="docs/animation/renders/backdrop.png" width=90% style="border: 5px solid #333"/></a>

***

### Table of Contents

- [Background](#background)
  - [Features](#features)
  - [Innovations](#innovations)
- [Production](#production)
  - [Assembly](#assembly)
  - [Installation](#installation)
- [Development](#development)
- [Contribution](#contribution)
  - [Roadmap](#roadmap)
- [Acknowledgements](#acknowledgements)

<!-- include some visuals, i.e. a 3D GIF of the CAD, maybe a flowchart/diagram -->

***

## Background

> See our [requirements document](https://github.com/PeaPodTechnologies/PeaPod/blob/staging/docs/design/requirements/Requirements.pdf) for details.

<br>

<img src="docs/design/solutionoverview/images/blackbox.png" width=90%/>

<br>

### Features

> See our [solution overview](https://github.com/PeaPodTechnologies/PeaPod/blob/staging/docs/design/solutionoverview/SolutionOverview.pdf) for details.

<br>

<img src="docs/design/solutionoverview/images/features.png" width=70%/>

<br>

### Innovations
PeaPod is *not* a fancy container with a light stand. 

PeaPod can **precisely** replicate climates for optimal growing conditions, and also has the ability to collect plant growth data during the growing cycle. 
This data is then used to further *optimize climate parameters* so food grown in PeaPods will taste better over time.

PeaPod is *modular* and *easy to assemble*. 
Construction with aluminum extrusion allows PeaPods to be transported like IKEA furniture, useful for situations where a lot of food is required at the end of a long journey. 
A lot of PeaPods can be collapsed into a tiny space. 
This also means PeaPods can be configured to be any size to grow any type of plant.

PeaPod is self-optimizing, versatile, easy to assemble, and it makes food.

***

## Production

### Assembly

See our [assembly documentation](/docs/assembly/ASSEMBLY.md) for details.

### Installation

1. Download the contents of [`software/`](software/) to the main computer (Raspberry PI)
2. Follow the software setup instructions in [`software/README`](software/README.md#Installation) to load Node.js dependencies and install the microcontroller (Arduino) code ([`software/PeaPod-Arduino`](software/PeaPod-Arduino))

<!-- ### Usage

WIP. -->

<!-- w/ examples, i.e. GIF of expected output, inline code -->

***

## Development
<!-- dependencies, environment, language -->

> See [`software/README`](software/README.md#Development) and [`hardware/README`](hardware/README.md#Development) for their respective dependencies.

<br>

## Contribution

### New Issue

1. Open an issue with the features you'd like to add/bugs you'd like to see squashed (admins will add labels, assignments, etc.)
2. Discuss the scope and specifics in the issue, and either:
   1. Make a new branch, make changes, open a **PR Draft** (if you have permissions); or
   2. Fork, make changes, open a **PR Draft**
3. Request review by changing the **PR Draft** to a full-blown **PR** when you think it's ready
4. If the reviewer(s) accept the changes, they will merge the **PR** and close the related issue

### Existing Issues

Check out our [open issues](https://github.com/PeaPodTechnologies/PeaPod/issues?q=is%3Aopen+is%3Aissue+label%3A%22Status%3A+Open%22)!

<!-- ### Support -->
<!-- issues page, discussion board/Discord? -->

### Roadmap
<!-- adapted from milestones/project boards -->

> See our [projects](https://github.com/PeaPodTechnologies/PeaPod/projects).

<br>

## Acknowledgements

Innumerable thanks to the following individuals and groups:
- PeaPod Technologies members ([here](https://github.com/orgs/PeaPodTechnologies/people))
- The Open Source community

PeaPod is great :) RIP Media Lab
