# FlaskBookBoilerplate v1.0 Roadmap

## Foundation

- [x] FBP-010 Repository/bootstrap
- [x] FBP-020 Application factory/config/extensions
- [x] FBP-030 Database foundation + migration-ready setup

## Identity

- [x] FBP-040 Authentication & accounts
  - [x] Registration / login / logout / remember-me
  - [x] Password hashing and change-password flow
  - [x] Safe post-login redirect
  - [x] Public/private profile boundary
  - [x] Administrator creation CLI
  - [x] Initial identity migration
- [x] FBP-050 RBAC
  - [x] User / Moderator / Administrator roles
  - [x] Permission IntFlag
  - [x] Permission decorator

## Catalogue

- [x] FBP-060 Books
  - [x] Authors
  - [x] Multiple authors
  - [x] ISBN-10 / ISBN-13
  - [x] Publisher
  - [x] Publication date
  - [x] Language
  - [x] Format
  - [x] Series + series number
  - [x] Covers (URL foundation)
  - [x] Archive / restore
- [x] FBP-070 Search
  - [x] Tags
  - [x] Categories
  - [x] Filters
  - [x] Sorting

## Personal library

- [ ] FBP-075 Personal library
  - [ ] Favorites
  - [ ] Want to read / read later
  - [ ] Currently reading
  - [ ] Read
  - [ ] Did not finish
  - [ ] Recently viewed
  - [ ] Custom collections
  - [ ] Reading dates
  - [ ] Optional reading progress

## Circulation

- [ ] FBP-080 Loans
  - [ ] Borrow
  - [ ] Return
  - [ ] Renew
  - [ ] Due dates
  - [ ] Overdue rules
- [ ] FBP-085 Reservations
  - [ ] Waiting list
  - [ ] Availability
  - [ ] Cancellation
  - [ ] Expiration

## Community

- [ ] FBP-090 Reviews
  - [ ] Rating 1–5
  - [ ] Review body
  - [ ] Editing
  - [ ] Soft delete
  - [ ] Moderation

## User experience

- [ ] FBP-095 Notifications
  - [ ] In-app
  - [ ] Due soon
  - [ ] Overdue
  - [ ] Reservation available
- [ ] FBP-100 Homepage
- [ ] FBP-105 Personal dashboard
- [ ] FBP-110 Media / avatars
- [ ] FBP-115 Metadata providers
  - [ ] Open Library
  - [ ] Google Books

## Interfaces / operations

- [ ] FBP-120 REST API v1
- [ ] FBP-130 Modern responsive UI
- [ ] FBP-140 CLI
- [ ] FBP-145 Administration dashboard
- [ ] FBP-150 Legacy database importer
- [ ] FBP-155 Audit log
- [ ] FBP-160 Docker
- [ ] FBP-170 CI hardening
- [ ] FBP-180 Security hardening
- [ ] FBP-190 Full legacy parity audit
- [ ] FBP-200 Boilerplate documentation/examples
- [ ] FBP-210 v1.0 release
