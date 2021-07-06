# PeaPod

A cloud-connected isolated and automated plant growth environment, able to generate any environment from a combination of independent environment parameters. 

Designed as both a hassle-free food production system and a research tool for precise and distributed mapping of the plant-environment relationship.

[![issuesopen](https://img.shields.io/github/issues/utagritech/peapod)](https://github.com/UTAgritech/PeaPod/issues) [![issuesclosed](https://img.shields.io/github/issues-closed/utagritech/peapod)](https://github.com/UTAgritech/PeaPod/issues?q=is%3Aissue+is%3Aclosed) [![opensource](https://img.shields.io/badge/open-source-red)](https://github.com/UTAgritech/PeaPod/projects) ![coffee](https://img.shields.io/badge/powered%20by-coffee-brown) ![24/7](https://img.shields.io/badge/Eat,%20Sleep,-PeaPod-darkgreen) [![FLDSMDFR](https://img.shields.io/badge/The-FLDSMDFR-orange)](https://www.youtube.com/watch?v=k8xFbWLUDoQ)

Intended as UTAG's submission to the [NASA/CSA Deep Space Food Challenge](https://impact.canada.ca/en/challenges/deep-space-food-challenge).

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

<!-- TODO: Build statuses? etc. -->

## Background

> See our [design brief](/docs/design/designbrief/DesignBrief.pdf) for details.

<img src="docs/design/solutionoverview/images/blackbox.png" width=90%/>

### Features

> See our [proposal](/docs/design/designproposal/DesignProposal.pdf) (WIP) for details.

<img src="docs/design/solutionoverview/images/features.png" width=70%/>


### Innovations
PeaPod is *not* a fancy container with a light stand. 

PeaPod can **precisely** replicate climates for optimal growing conditions, and also has the ability to collect plant growth data during the growing cycle. 
This data is then used to further *optimize climate parameters* so food grown in PeaPods will taste better over time.

PeaPod is *modular* and *easy to assemble*. 
Construction with aluminum extrusion allows PeaPods to be transported like IKEA furniture, useful for situations where a lot of food is required at the end of a long journey. 
A lot of PeaPods can be collapsed into a tiny space. 
This also means PeaPods can be configured to be any size to grow any type of plant.

PeaPod is self-optimizing, versatile, easy to assemble, and it makes food.
## Production

### Assembly

See our [assembly documentation](/docs/assembly/ASSEMBLY.md) for details.

### Installation

1. Download the contents of [`software/`](software/) to the main computer (Raspberry PI)
2. Follow the software setup instructions in [`software/README`](software/README.md#Installation) to load Node.js dependencies and install the microcontroller (Arduino) code ([`software/PeaPod-Arduino`](software/PeaPod-Arduino))

<!-- ### Usage

WIP. -->

<!-- w/ examples, i.e. GIF of expected output, inline code -->

## Development
<!-- dependencies, environment, language -->

> See [`software/README`](software/README.md#Development) and [`hardware/README`](hardware/README.md#Development) for their respective dependencies.

## Contribution

1. Open an issue with the features you'd like to add/bugs you'd like to see squashed (admins will add labels, assignments, etc.)
2. Discuss the scope and specifics in the issue, and either:
   1. Make a new branch, make changes, open a **PR Draft** (if you have permissions); or
   2. Fork, make changes, open a **PR Draft**
3. Request review by changing the **PR Draft** to a full-blown **PR** when you think it's ready
4. If the reviewer(s) accept the changes, they will merge the **PR** and close the related issue

<!-- ### Support -->
<!-- issues page, discussion board/Discord? -->

### Roadmap
<!-- adapted from milestones/project boards -->

> See our [projects](https://github.com/UTAgritech/PeaPod/projects).

## Acknowledgements

Innumerable thanks to the following individuals and groups:
- University of Toronto Agritech's active members ([here](https://github.com/orgs/UTAgritech/people))
- The Open Source community

PeaPod is great :) RIP Media Lab