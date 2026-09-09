# Legacy BookLibrary parity contract

The legacy repository is a functional reference only. FlaskBookBoilerplate is a
new implementation.

## Behaviors to preserve

### Accounts

- Register.
- Log in / log out.
- Remember-me.
- Change password.
- Profile.
- Avatar.
- Role and permission model.

### Books

- Catalogue.
- Detail.
- Search.
- Tags.
- Availability.
- Add / edit.
- Archive / restore.
- Pagination.

### Loans

- Default loan duration: 30 days.
- Borrow an available book.
- Do not allow the same user to hold the same book twice simultaneously.
- Block new borrowing when the user has an overdue active loan.
- Return the correct active loan.
- Preserve loan history.

### Reviews / comments

- User-authored book feedback.
- Owner removal.
- Moderator/admin moderation.
- Historical soft-delete behavior is preserved semantically, with improved audit fields.

### Homepage / discovery

- Popular books.
- Popular users/readers where retained by product design.
- Recent reviews/comments.

### API

Legacy read capabilities for:

- books
- users
- tags
- loans/logs
- comments/reviews

are represented in `/api/v1`, with corrected privacy and archive behavior.

## Known legacy defects that must become regression tests

- Archived books can reappear after a query is overwritten.
- Return-book route checks the wrong variable.
- Return-book redirect can use a loan id as a book id.
- Return-by-book can select an already returned historical loan.
- Destructive actions are exposed as GET routes.
- Archived books can leak through list API endpoints.
- Pagination links use inconsistent parameter names.
- Nested API links can use a book id where a user id is expected.
- Some datetime defaults are evaluated at import time.
- Profile form contains an incorrectly cased validators argument.
- `next` redirects are not sufficiently validated.
- Production secret/admin identity are hard-coded.
- Demo accounts and database seeding are triggered implicitly by application startup.

## Intentional v1 extensions beyond legacy

- Favorites.
- Read later / want to read.
- Reading states.
- Recently viewed.
- Custom collections.
- Ratings/reviews.
- Renewals.
- Reservations.
- Notifications.
- Multiple authors.
- Structured book metadata.
- External metadata providers.
- Audit log.
